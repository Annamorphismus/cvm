#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <optional>
#include <random>
#include <vector>

namespace cvm {

[[nodiscard]] inline auto random_sample(double p) noexcept -> bool
{
    if(p < 0.0) [[unlikely]] {
        return false;
    }

    if(p >= 1.0) [[unlikely]] {
        return true;
    }

    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::uniform_real_distribution<> dist(0.0, 1.0);

    return dist(gen) < p;
}

template<class Iter>
[[nodiscard]] static auto naive_cvm(Iter begin, Iter end, double EPSILON, double DELTA) noexcept
    -> std::optional<double>
{
    using ItemType = typename std::iterator_traits<Iter>::value_type;

    const auto number_of_elements = std::distance(begin, end);
    const std::size_t THRESHOLD = (12. / EPSILON * EPSILON) * std::log2((8 * number_of_elements) / DELTA);

    double p = 1;
    std::vector<ItemType> Xi;

    for(auto it = begin; it != end; ++it) {
        std::erase(Xi, *it);

        if(random_sample(p)) {
            Xi.emplace_back(*it);
        }

        if(Xi.size() >= THRESHOLD) {
            std::erase_if(Xi, [p](auto /*_*/) { return random_sample(0.5); });
            p /= 2;

            if(Xi.size() >= THRESHOLD) {
                return std::nullopt;
            }
        }
    }

    return Xi.size() / p;
}

} // namespace cvm
