#include "lossy_link.h"

bool LossyLink::drop(float ratio)
{
    std::lock_guard<std::mutex> lock(_mutex);

    std::uniform_real_distribution<> distribution(0.0f, 1.0f);
    const float random_value = distribution(_engine);
    const bool should_drop = (random_value < ratio);
    return should_drop;
}
