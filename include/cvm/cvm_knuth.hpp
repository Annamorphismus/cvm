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


    // knuth version des cvm algorithmus mit treap
template<class Iter>
[[nodiscard]] static auto knuth_cvm(Iter begin, Iter end, std::size_t s) noexcept
    -> std::optional<double>
{
    // typ der elemente des streams
    using ItemType = typename std::iterator_traits<Iter>::value_type;

    // typ des treaps, Key=ItemType Prio=double
    using TreapType = Treap<ItemType, double>;

    double p = 1;

    // initialisiere leeren treap
    TreapType B;


    //iteriere über die elemente des streams
    for(auto it = begin; it != end; ++it) {

        // lösche des neue element aus dem treap
        B.delete_elem(*it);

        // generiere eine neue priority für den heap
        const auto u = TreapType::generate_prio();

        if(u >= p) {
            // neue prop größer als p -> tue nichts
            continue;
        }

        // wenn treap kleiner als s kann das element mit prio u eingefügt werden
        if(B.size() < s) {
            B.insert(*it, u);
            continue;
        }

        // top element im heap anschauen
        // .top gibt den root i treap zurück
        // .value() entpackt das optional von std::optional<std::pair<K, P>> zu std::pair<K,P>
        const auto [a_prime, u_prime] = B.top().value();

        if(u >= u_prime) {
            p = u;
        } else {
            // top element wird aus dem heap gelöscht
            B.pop();

            // neues lement wird eingefügt
            B.insert(*it, u);

            // p wird upgedated auf die prio des gelöschten top elements
            p = u_prime;
        }
    }

    return B.size() / p;
}

} // namespace cvm
