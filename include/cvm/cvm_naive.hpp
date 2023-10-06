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

// Die Funktion random_sample verwendet den übergebenen Wahrscheinlichkeitswert p, um eine zufällige Entscheidung zu treffen und true oder false zurückzugeben, wobei die Wahrscheinlichkeit von true gleich p ist.
[[nodiscard]] inline auto random_sample(double p) noexcept -> bool
{
    // Wenn p kleiner als 0 ist, gibt die Funktion sofort 'false' zurück.
    if(p < 0.0) [[unlikely]] {
        return false;
    }

    // Wenn p größer oder gleich 1 ist, gibt die Funktion sofort 'true' zurück.
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
    // Ermitteln des Datentyps der Elemente im übergebenen Stream.
    using ItemType = typename std::iterator_traits<Iter>::value_type;

    // Bestimmt die Anzahl der Elemente im Stream.
    const auto number_of_elements = std::distance(begin, end);

    // Berechnung des treshs
    const std::size_t THRESHOLD = (12. / EPSILON * EPSILON) * std::log2((8 * number_of_elements) / DELTA);
    // Initialisierung von p und X
    double p = 1;
    std::vector<ItemType> X;

    // Iteriere über die Elemente des Streams
    for(auto it = begin; it != end; ++it) {

        // Lösche das neue Element aus X
        std::erase(X, *it);

        // Mit Wahrscheinlichkeit p wird das Element wieder eingefügt
        if(random_sample(p)) {
            X.emplace_back(*it);
        }

        // Überprüfen, ob die Größe von X den festgelegten Schwellenwert erreicht oder überschreitet.
        if(X.size() >= THRESHOLD) {

            // Entfernt jedes Element in X mit einer Wahrscheinlichkeit von 0.5.
            std::erase_if(X, [p](auto /*_*/) { return random_sample(0.5); });

            // Aktualisieren von p auf die Hälfte seines aktuellen Werts.
            p /= 2;

            // Überprüfen, ob die Größe von X immer noch über dem Schwellenwert liegt; falls ja, wird ein Error zurückgeben.
            if(X.size() >= THRESHOLD) {
                return std::nullopt;
            }
        }
    }

    return X.size() / p;
}

} // namespace cvm
