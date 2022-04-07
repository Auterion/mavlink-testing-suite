#include <gtest/gtest.h>
#include "../environment.hpp"
#include <future>
#include <memory>
#include <vector>
#include <random>
#include <filesystem>
#include <fstream>

using namespace MavlinkTestingSuite;

class FTPSDK : public ::testing::Test {
protected:
    const std::string OUT_DIR = "mavlink_testing_suite_out";
    const std::string IN_DIR = "mavlink_testing_suite_in";
    const std::string FILENAME = "dummy_data.bin";

    const std::shared_ptr<mavsdk::Ftp> ftp;
    const YAML::Node config;
    const size_t file_size;
    const std::string target_path;
    std::filesystem::path _temp_dir;
    std::filesystem::path _out_file;
    std::filesystem::path _out_file_corrupted;

    std::vector<char> data;


    FTPSDK() :
          ftp(Environment::getInstance()->getFtpPlugin()),
          config(Environment::getInstance()->getConfig({"FTP"})),
          file_size(config["file_size"].as<size_t>()),
          target_path(config["target_path"].as<std::string>())
    {
        initRandomData();
        writeTempFiles();
    }

    ~FTPSDK() override {
        //removeTempFilesIfExists();
    }

    void initRandomData() {
        std::mt19937 mt_gen{42};
        data.resize(file_size);
        int remaining = file_size;
        while (remaining > 0) {
            uint32_t val = mt_gen();
            for (int i=0; i<4 && remaining > 0; i++) {
                data[file_size - remaining + i] = (val >> (8*i)) & 0xFF;
                remaining -= 1;
            }
        }
    }

    void removeTempFilesIfExists() {
        if (std::filesystem::exists(_temp_dir / OUT_DIR)) {
            std::filesystem::remove_all(_temp_dir / OUT_DIR);
        }
        if (std::filesystem::exists(_temp_dir / IN_DIR)) {
            std::filesystem::remove(_temp_dir / IN_DIR);
        }
    }

    void writeTempFiles() {
        removeTempFilesIfExists();

        _temp_dir = std::filesystem::temp_directory_path();
        std::filesystem::create_directory(_temp_dir / OUT_DIR);
        std::filesystem::create_directory(_temp_dir / IN_DIR);

        _out_file = _temp_dir / OUT_DIR / FILENAME;
        {
            std::ofstream f;
            f.open(_out_file, std::ios::out | std::ios::binary);
            f.write(data.data(), file_size);
            f.close();
        }
        _out_file_corrupted = _temp_dir / OUT_DIR / ("corrupted_" + FILENAME);
        {
            std::ofstream f;
            f.open(_out_file_corrupted, std::ios::out | std::ios::binary);
            f.write(data.data(), file_size-1);
            f.close();
        }
    }

    bool checkFilesEqual(const std::filesystem::path &f1, const std::filesystem::path &f2) {
        std::ifstream if1, if2;
        if1.open(f1, std::ios::binary);
        if2.open(f2, std::ios::binary);
        bool success = true;
        while (if1 && if2) {
            char c1, c2;
            if1.get(c1);
            if2.get(c2);
            if (c1 != c2) {
                success = false;
                break;
            }
        }
        success = success && !if1 && !if2; // both files have to be finished
        if1.close();
        if2.close();
        return success;
    }

};



TEST_F(FTPSDK, UploadCompareDownloadCompare) {
    auto conf = Environment::getInstance()->getConfig({"FTPSDK", "UploadCompareDownloadCompare"});
    if (!conf || conf["skip"].as<bool>(false)) {
        GTEST_SKIP();
    }

    {
        auto prom = std::promise<mavsdk::Ftp::Result>{};
        auto future = prom.get_future();
        ftp->upload_async(
            _out_file.c_str(), target_path,
            [&prom](const mavsdk::Ftp::Result result, const mavsdk::Ftp::ProgressData& progress) {
                (void)progress;
                if (result != mavsdk::Ftp::Result::Next) {
                    prom.set_value(result);
                }
            });

        future.wait_for(std::chrono::seconds(5));
        auto res = future.get();
        ASSERT_EQ(res, mavsdk::Ftp::Result::Success);
    }

    {
        auto res = ftp->are_files_identical(_out_file.c_str(), target_path + FILENAME);
        ASSERT_EQ(res.first, mavsdk::Ftp::Result::Success);
        ASSERT_EQ(res.second, true);
    }

    {
        auto res = ftp->are_files_identical(_out_file_corrupted.c_str(), target_path + FILENAME);
        ASSERT_EQ(res.first, mavsdk::Ftp::Result::Success);
        ASSERT_EQ(res.second, false);
    }

    {
        auto prom = std::promise<mavsdk::Ftp::Result>{};
        auto future = prom.get_future();
        ftp->download_async(
            target_path + FILENAME, (_temp_dir / IN_DIR).c_str(),
            [&prom](const mavsdk::Ftp::Result result, const mavsdk::Ftp::ProgressData& progress) {
                (void)progress;
                if (result != mavsdk::Ftp::Result::Next) {
                    prom.set_value(result);
                }
            });

        future.wait_for(std::chrono::seconds(5));
        auto res = future.get();
        ASSERT_EQ(res, mavsdk::Ftp::Result::Success);

        bool files_equal = checkFilesEqual(_out_file, _temp_dir / IN_DIR / FILENAME);
        ASSERT_EQ(files_equal, true);
    }

    {
        auto res = ftp->remove_file(target_path + FILENAME);
        ASSERT_EQ(res, mavsdk::Ftp::Result::Success);
    }
}
