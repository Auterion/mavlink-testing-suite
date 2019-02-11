#pragma once

#include <random>

class LossyLink
{
public:
    /* Checks if we should drop a message given a ratio of dropped messages.
     *
     * @param ratio Ratio of drops (0: no drops, 1: everything dropped)
     * @return true if message should be dropped.
     */
    bool drop(float ratio);

private:
    // We need predictable results, so we always start with the same seed.
    std::seed_seq _seed{0};
    std::default_random_engine _engine{_seed};
};
