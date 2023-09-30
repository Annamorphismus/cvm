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

// returns true with probability p
[[nodiscard]] inline auto random_sample(double p) noexcept -> bool
{
    if(p < 0.0) [[unlikely]] {
        return false;
    }

    if(p >= 1.0) [[unlikely]] {
        return true;
    }

    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<> dist(0.0, 1.0);

    return dist(gen) < p;
}

template<class Iter>
[[nodiscard]] static auto naive_cvm(Iter begin, Iter end, double EPSILON, double DELTA) noexcept
    -> std::optional<double>
{
    // der typ der einzelnen elemente im stream
    using ItemType = typename std::iterator_traits<Iter>::value_type;

    // nummer der elemente im stream
    const auto number_of_elements = std::distance(begin, end);

    // THRESH
    const std::size_t THRESHOLD = (12. / EPSILON * EPSILON) * std::log2((8 * number_of_elements) / DELTA);

    double p = 1;
    std::vector<ItemType> X;

    //iteriere über die elemente des streams
    for(auto it = begin; it != end; ++it) {

        // lösche das neue element aus X
        std::erase(X, *it);

        // mit wahrscheinlichkeit p wird das element wieder eingefügt
        if(random_sample(p)) {
            X.emplace_back(*it);
        }

        // wenn X >= Thresh
        if(X.size() >= THRESHOLD) {

            // jedes element wird mit wahrscheinlichkeit 0.5 gelöscht
            std::erase_if(X, [p](auto /*_*/) { return random_sample(0.5); });

            // update p auf p/2
            p /= 2;

            // wenn X >= Thresh return ERROR
            if(X.size() >= THRESHOLD) {
                return std::nullopt;
            }
        }
    }

    return X.size() / p;
}

} // namespace cvm
