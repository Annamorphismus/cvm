#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <cvm/cvm_knuth.hpp>
#include <cvm/cvm_naive.hpp>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

// Funktion, um einen zufälligen Vektor von Integral-Typen der Größe N zu erzeugen
template<class T>
[[nodiscard]] inline auto random_vec(std::size_t N) noexcept -> std::vector<T>
{
    static_assert(std::is_integral_v<T>, "T should be an integral type");

    std::vector<T> arr(N);

    // Lambda Funktion wird definiert und aufgerufen
    // Dies erzeugt und gibt einen Zufallszahlengenerator zurück.
    auto gen = []() constexpr {
        // Überprüft, ob der Typ T identisch mit std::uint64_t ist.
        if constexpr(std::is_same_v<T, std::uint64_t>) {
            // Wenn T ein std::uint64_t ist, wird ein 64-Bit-Zufallszahlengenerator verwendet.
            // std::mt19937_64 ist eine Implementierung des Mersenne-Twister-Algorithmus mit 64 Bits.
            // std::random_device wird verwendet, um einen zufälligen Seed für den Zufallszahlengenerator zu erzeugen.
            return std::mt19937_64(std::random_device{}());
        } else {
            // Für alle anderen Typen von T wird ein 32-Bit-Zufallszahlengenerator verwendet.
            // std::mt19937 ist eine Implementierung des Mersenne-Twister-Algorithmus mit 32 Bits.
            // Auch hier wird std::random_device für den Seed verwendet.
            return std::mt19937(std::random_device{}());
        }
    }();

    // Erzeugt eine gleichmäßige Verteilung für ganze Zahlen.
    // Diese Verteilung ist so definiert, dass sie Zahlen im gesamten gültigen Bereich des Typs T produzieren kann.
    std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(),
                                          std::numeric_limits<T>::max());

    // Befüllen des Vektors mit zufällig gleichverteilten Werten
    for(auto& elem : arr) {
        elem = dist(gen);
    }

    return arr;
}

// Template-Funktion zur Durchführung des naiven CVM-Benchmarks.
template<class T>
inline static auto naive(benchmark::State& state)
{
    // Extrahiert den ersten Wert aus den bereitgestellten Benchmark-Argumenten und konvertiert ihn zu einem double.
    auto N = static_cast<double>(state.range(0));

    // Extrahiert den zweiten Wert und konvertiert ihn zu einem double.
    auto i = static_cast<double>(state.range(1));

    // Extrahiert den dritten Wert und konvertiert ihn zu einem std::size_t.
    auto j = static_cast<std::size_t>(state.range(2));

    // Berechnet den Wert von eps auf Basis des zuvor extrahierten Wertes i.
    double eps = static_cast<double>(i) / 10.;

    // Berechnet den Wert von delta auf Basis des zuvor extrahierten Wertes j.
    double delta = static_cast<double>(j) * 0.0001;


    // Dies ist die Haupt-Benchmark-Schleife. Sie wird so oft durchlaufen, wie von der Google Benchmark Library benötigt, um zuverlässige Zeitmessdaten zu erhalten.
    for(auto _ : state) {
        // Pausiert die Zeitmessung. Dies ist nützlich, wenn man bestimmte Teile des Codes aus der eigentlichen Benchmark-Zeitmessung ausschließen möchte.
        state.PauseTiming();

        // Generiert einen zufälligen Vektor der Größe N.
        auto vec = random_vec<T>(N);

        // Setzt die Zeitmessung fort.
        state.ResumeTiming();

        // Ruft den naive_cvm-Algorithmus aus dem cvm-Namensraum auf und speichert das Ergebnis.
        auto result = cvm::naive_cvm(std::begin(vec), std::end(vec), eps, delta);

        // Instruiert die Benchmarking-Bibliothek, das Ergebnis nicht zu optimieren.
        benchmark::DoNotOptimize(result);
    }
}

// Template-Funktion zur Durchführung des Knuth CVM-Benchmarks.
template<class T>
inline static auto knuth(benchmark::State& state)
{

    // Extrahiert den ersten Wert aus den bereitgestellten Benchmark-Argumenten und konvertiert ihn zu einem double.
    auto N = static_cast<double>(state.range(0));

    // Extrahiert den zweiten Wert aus den Benchmark-Argumenten und konvertiert ihn zu einem double.
    auto s = static_cast<double>(state.range(1));

    // Dies ist die Haupt-Benchmark-Schleife. Sie wird so oft durchlaufen, wie von der Google Benchmark Library benötigt, um zuverlässige Zeitmessdaten zu erhalten.
    for(auto _ : state) {
        // Pausiert die Zeitmessung.
        state.PauseTiming();

        // Generiert einen zufälligen Vektor der Größe N.
        auto vec = random_vec<T>(N);

        // Setzt die Zeitmessung fort, um den eigentlichen Algorithmus, den wir messen möchten, zu erfassen.
        state.ResumeTiming();

        // Führt den knuth_cvm-Algorithmus aus dem cvm-Namensraum aus und speichert das Ergebnis.
        // Dies ist der eigentliche zu benchmarkende Code.
        auto result = cvm::knuth_cvm(std::begin(vec), std::end(vec), s);

        // Instruiert die Benchmarking-Bibliothek, das Ergebnis `result` nicht zu optimieren.
        benchmark::DoNotOptimize(result);
    }
}

// Funktion, die benutzerdefinierte Argumente für den Naiven Benchmark festlegt.
static void CustomArgumentsNaive(benchmark::internal::Benchmark* b)
{

    // Äußerste Schleife variiert N von 1 bis 10.000.000 in Zehnerpotenzen.
    for(std::int64_t N = 1; N <= 10000000; N *= 10) {
        for(std::int64_t i = 1; i <= 10; i += 2) {
            for(std::int64_t j = 1; j <= 1000; j *= 10) {
                double epsilon = static_cast<double>(i) / 10.;
                double delta = static_cast<double>(j) * 0.0001;

                // Berechnung von 'thresh'
                std::int64_t thresh = (12. / epsilon * epsilon) * std::log2((8. * static_cast<double>(N)) / delta);

                // Des Weiteren wurden Kombinationen ausgeschlossen, bei denen der Schwellwert die Gesamtanzahl der Elemente im Datenstrom übersteigt.
                if(thresh < N) {
                    b->Args({N, i, j});
                }
            }
        }
    }
}
// Funktion, die benutzerdefinierte Argumente für den Knuth-Benchmark festlegt.
static void CustomArgumentsKnuth(benchmark::internal::Benchmark* b)
{
    for(std::int64_t N = 1; N <= 10000000; N *= 10) {
        for(std::int64_t s = 1; s <= N; s *= 10) {
            // Fügt jedes Argumentpaar (N, s) zum Benchmark hinzu.
            b->Args({N, s});
        }
    }
}

// BENCHMARK(knuth<std::uint8_t>)->Apply(CustomArgumentsKnuth);
// BENCHMARK(knuth<std::uint16_t>)->Apply(CustomArgumentsKnuth);
// BENCHMARK(knuth<std::uint32_t>)->Apply(CustomArgumentsKnuth);
// BENCHMARK(knuth<std::uint64_t>)->Apply(CustomArgumentsKnuth);

BENCHMARK(naive<std::uint8_t>)->Apply(CustomArgumentsNaive);
BENCHMARK(naive<std::uint16_t>)->Apply(CustomArgumentsNaive);
BENCHMARK(naive<std::uint32_t>)->Apply(CustomArgumentsNaive);
BENCHMARK(naive<std::uint64_t>)->Apply(CustomArgumentsNaive);

// main function
BENCHMARK_MAIN();
