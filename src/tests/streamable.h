#pragma once

#include <sstream>

namespace tests
{
// SFINAE to check if something is streamable adapted from:
// https://stackoverflow.com/questions/29319000
template <class...>
struct Voider {
    using type = void;
};
template <class... T>
using void_t = typename Voider<T...>::type;

template <class T, class = void>
struct MaybeStreamable {
    std::string print(const T& thing)
    {
        (void)(thing);
        return "";
    }
    bool isStreamable() const { return false; }
};

template <class T>
struct MaybeStreamable<T, void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> {
    std::string print(const T& thing)
    {
        std::stringstream ss;
        ss << thing;
        return ss.str();
    }

    bool isStreamable() const { return true; }
};

}  // namespace tests
