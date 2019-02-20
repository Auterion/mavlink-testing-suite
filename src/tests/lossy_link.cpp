#include "lossy_link.h"

LossyLink::LossyLink(int seed_num) : _engine(seed_num) {}

bool LossyLink::drop(float ratio)
{
    std::uniform_real_distribution<> distribution(0.0f, 1.0f);
    const float random_value = distribution(_engine);
    const bool should_drop = (random_value < ratio);
    return should_drop;
}
