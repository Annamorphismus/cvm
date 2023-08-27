#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <optional>
#include <random>
#include <cvm/treap.hpp>

namespace cvm {

template<class Iter>
[[nodiscard]] static auto knuth_cvm(Iter begin, Iter end, std::size_t s) noexcept
    -> std::optional<double>
{
    using ItemType = typename std::iterator_traits<Iter>::value_type;
    using TreapType = Treap<ItemType, double>;

    double p = 1;
    TreapType B;

    for(auto it = begin; it != end; ++it) {
        B.delete_elem(*it);

        const auto u = TreapType::generate_prio();

        if(u >= p) {
            continue;
        }

        if(B.size() < s) {
            B.insert(*it, u);
            continue;
        }

        const auto [a_prime, u_prime] = B.top().value();

        if(u >= u_prime) {
            p = u;
        } else {
            B.pop();
            B.insert(*it, u);
            p = u_prime;
        }
    }

    return B.size() / p;
}

} // namespace cvm
