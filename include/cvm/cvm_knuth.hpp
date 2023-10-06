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


// Knuth Version des CVM-Algorithmus mit Treap
template<class Iter>
[[nodiscard]] static auto knuth_cvm(Iter begin, Iter end, std::size_t s) noexcept
    -> std::optional<double>
{
    // Typ der Elemente des Streams
    using ItemType = typename std::iterator_traits<Iter>::value_type;

    // Typ des Treaps, Key=ItemType Prio=double
    using TreapType = Treap<ItemType, double>;

    double p = 1;

    // Initialisiere leeren Treap
    TreapType B;


    // Iteriere über die Elemente des Streams
    for(auto it = begin; it != end; ++it) {

        // Lösche des neue Element aus dem Treap
        B.delete_elem(*it);

        // Generiere eine neue priority für den Heap
        const auto u = TreapType::generate_prio();

        if(u >= p) {
            // Neue prio größer als p -> tue nichts
            continue;
        }

        // Wenn Treap kleiner als s kann das Element mit prio u eingefügt werden
        if(B.size() < s) {
            B.insert(*it, u);
            continue;
        }

        // Top Element im Heap anschauen
        // .top gibt den Root i Treap zurück
        // .value() entpackt das optional von std::optional<std::pair<K, P>> zu std::pair<K,P>
        const auto [a_prime, u_prime] = B.top().value();

        if(u >= u_prime) {
            p = u;
        } else {
            // Top Element wird aus dem Heap gelöscht
            B.pop();

            // Neues Element wird eingefügt
            B.insert(*it, u);

            // p wird upgedated auf die Prio des gelöschten Top Elements
            p = u_prime;
        }
    }

    return B.size() / p;
}

} // namespace cvm
