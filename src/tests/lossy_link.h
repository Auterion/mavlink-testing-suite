#pragma once

#include <random>

class LossyLink
{
public:
    explicit LossyLink(int seed_num);
    ~LossyLink() = default;

    /** Checks if we should drop a message given a ratio of dropped messages.
     *
     * @param ratio Ratio of drops (0: no drops, 1: everything dropped)
     * @return true if message should be dropped.
     */
    bool drop(float ratio);

private:
    std::default_random_engine _engine;
};
