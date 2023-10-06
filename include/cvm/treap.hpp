#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <random>
#include <type_traits>

namespace cvm {

// Implementierung eines Treap mit Schlüsseln vom Typ K und Prioritäten vom Typ P.
// Standardmäßig ist der Typ für Prioritäten ein Double.
template<class K, class P = double>
class Treap
{
public:
    // Standardkonstruktor, der einen leeren Treap erstellt.
    constexpr Treap() noexcept
        : root_(nullptr) // Wurzelknoten wird auf nullptr gesetzt.
    {
    }

    // Destruktor, der den Treap und all seine Knoten zerstört.
    ~Treap() noexcept
    {
        destroy(root_);
    }

    // Einfügemethode, die ein Element in den Treap einfügt,
    // wobei die Priorität zufällig generiert wird.
    auto insert(const K& elem) noexcept -> void
    {
        const auto prio = generate_prio();
        insert(elem, prio);
    }

    // Einfügemethode, die ein Element mit einer gegebenen Priorität in den Treap einfügt.
    auto insert(K elem, P prio) noexcept -> void
    {
        // Diese Hilfslambda führt das rekursive Einfügen durch.
        constexpr static auto insert_recursive =
            [&](auto& self, auto* node, K&& elem, P prio) -> Node* {
            if(!node) {
                return new Node{elem, prio};
            }

            if(elem < node->elem) {
                node->left = self(self, node->left, std::move(elem), prio);
                if(node->left->prio > node->prio) {
                    node = rotate_right(node);
                }
            } else if(elem > node->elem) {
                node->right = self(self, node->right, std::move(elem), prio);
                if(node->right->prio > node->prio) {
                    node = rotate_left(node);
                }
            }

            update_size(node);

            return node;
        };

        root_ = insert_recursive(insert_recursive, root_, std::move(elem), prio);
    }

    // Überprüft, ob ein Element mit Schlüssel K im Treap vorhanden ist.
    [[nodiscard]] auto contains(const K& elem) const noexcept -> bool
    {
        // Hilfslambda für die rekursive Suche.
        constexpr static auto contains_recursive =
            [&](auto& self, const auto* const node, const K& queryElem) -> bool {
            // clang-format off
            if(!node) return false;
            // clang-format on

            if(node->elem == queryElem) {
                return true;
            }

            if(queryElem < node->elem) {
                return self(self, node->left, queryElem);
            }

            return self(self, node->right, queryElem);
        };

        return contains_recursive(contains_recursive, root_, elem);
    }

    // Methode zum Löschen eines Elements mit dem gegebenen Schlüssel K aus dem Treap.
    auto delete_elem(const K& elem) noexcept
    {
        // Eine Hilfs-Lambda-Funktion für die rekursive Löschung.
        constexpr static auto delete_recursive =
            [&](auto& self, auto* node, const K& elem) -> Node* {
            // Base case: Knoten nicht gefunden, gib null zurück.
            // clang-format off
            if(!node) return nullptr;
            // clang-format on

            // Wenn der zu löschende Knoten gefunden wird.
            if(elem == node->elem) {

                // Wenn der Knoten beide Kinder hat, entscheide die Rotation basierend auf der Priorität.
                if(node->left && node->right) {
                    if(node->left->prio > node->right->prio) {
                        // Rotation nach rechts
                        node = rotate_right(node);
                        node->right = self(self, node->right, elem);
                    } else {
                        // Rotation nach links
                        node = rotate_left(node);
                        node->left = self(self, node->left, elem);
                    }
                } else {
                    // Knoten mit nur einem Kind oder keinem Kind.
                    Node* temp;
                    if(!node->left) {
                        temp = node->right;
                        delete node;
                        return temp;
                    }

                    if(!node->right) {
                        temp = node->left;
                        delete node;
                        return temp;
                    }
                }
            } else if(elem < node->elem) {
                // Wenn das zu löschende Element kleiner als das aktuelle Knotenelement ist.
                node->left = self(self, node->left, elem);
            } else {
                // Wenn das zu löschende Element größer als das aktuelle Knotenelement ist.
                node->right = self(self, node->right, elem);
            }

            // Größe aktualisieren.
            update_size(node);

            return node;
        };

        root_ = delete_recursive(delete_recursive, root_, elem);
    }

    // pop wird implementiert, indem der Root entfernt wird und dann der linke und rechte Teilbaum des Root Knotens zusammengeführt werden mit join()
    constexpr auto pop() noexcept -> std::optional<std::pair<K, P>>
    {
        // Überprüfe, ob der Wurzelknoten leer ist
        // clang-format off
        if(!root_) return std::nullopt;
        // clang-format on

        // Hole den Wert des Wurzelknotens
        auto root_value = std::move(root_->elem);
        auto prio = root_->prio;

        // Speichere Zeiger auf linke und rechte Kinder
        auto* const leftChild = root_->left;
        auto* const rightChild = root_->right;

        // Lösche den Wurzelknoten
        delete root_;

        // Erstelle einen neuen Treap aus dem linken und rechten Unterbaum
        root_ = join(leftChild, rightChild);

        return std::pair{std::move(root_value), prio};
    }

    [[nodiscard]] constexpr auto top() const noexcept -> std::optional<std::pair<K, P>>
    {
        // Überprüfe, ob der Wurzelknoten leer ist
        // clang-format off
        if(!root_) return std::nullopt;
        // clang-format on

        return std::pair{root_->elem, root_->prio};
    }

    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
    {
        // Gibt die Größe des Treaps zurück.
        // Wenn der Wurzelknoten (root_) existiert, gibt die Größe dieses Knotens zurück.
        // Andernfalls, wenn der Wurzelknoten nicht existiert, gibt 0 zurück.
        return root_ ? root_->size : 0;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        // Überprüft, ob der Treap leer ist.
        // Gibt 'true' zurück, wenn der Wurzelknoten nicht existiert (d.h. der Treap ist leer).
        // Andernfalls gibt 'false' zurück.
        return not static_cast<bool>(root_);
    }

    constexpr auto clear() noexcept -> void
    {
        // Löscht alle Knoten des Treaps und setzt den Wurzelknoten auf nullptr.
        // Die Methode 'destroy' wird aufgerufen, um alle Knoten rekursiv zu löschen.
        destroy(root_);
        root_ = nullptr;
    }

    [[nodiscard]] static auto generate_prio() noexcept -> P
    {
        // Generiert eine Priorität für einen Treap-Knoten.
        // 'rd' ist ein Zufallsgenerator, der zur Initialisierung von 'gen' (Mersenne Twister) verwendet wird.
        static std::random_device rd{};

        // 'gen' ist ein Mersenne-Twister-Zufallsgenerator.
        static std::mt19937 gen = std::mt19937{rd()};

        // Abhängig davon, ob der Typ P ein Gleitkomma-Typ ist,
        // wird entweder eine Gleichverteilung für Gleitkommazahlen oder Ganzzahlen erstellt.
        static std::conditional_t<std::is_floating_point_v<P>,
                                  std::uniform_real_distribution<>,
                                  std::uniform_int_distribution<>>
            dist{};

        // Gibt einen zufälligen Wert aus der gewählten Verteilung zurück.
        return dist(gen);
    }

private:
    // Struktur, die einen Knoten im Treap repräsentiert.
    struct Node
    {
        K elem;                // Schlüsselwert des Knotens.
        P prio;                // Priorität des Knotens.
        Node* left = nullptr;  // Zeiger auf den linken Kindknoten.
        Node* right = nullptr; // Zeiger auf den rechten Kindknoten.
        std::size_t size = 1;  // Größe des Subbaums, der an diesem Knoten hängt.
    };

    // Aktualisiert die Größe eines gegebenen Knotens basierend auf der Größe seiner Kinder.
    constexpr static auto update_size(Node* x) noexcept
    {
        x->size = 1 + (x->left ? x->left->size : 0) + (x->right ? x->right->size : 0);
    }

    // Führt eine Linksdrehung für einen gegebenen Knoten durch.
    constexpr static auto rotate_left(Node* x) noexcept -> Node*
    {
        auto* const y = x->right;
        x->right = y->left;
        y->left = x;

        // Aktualisiert die Größen der Knoten nach der Rotation.
        update_size(x);
        update_size(y);

        return y; // Gibt den neuen Wurzelknoten nach der Drehung zurück
    }

    // Führt eine Rechtsdrehung für einen gegebenen Knoten durch.
    constexpr static auto rotate_right(Node* y) noexcept -> Node*
    {
        auto* const x = y->left;
        y->left = x->right;
        x->right = y;

        // Aktualisiert die Größen der Knoten nach der Rotation.
        update_size(x);
        update_size(y);

        return x; // Gibt den neuen Wurzelknoten nach der Drehung zurück.
    }

    // Verbindet zwei Subbäume (left und right) in einen einzigen Baum, wobei die Prioritäten beachtet werden.
    constexpr static auto join(Node* left, Node* right) noexcept -> Node*
    {
        // clang-format off
        if(!left)  return right; // Wenn der linke Subbaum null ist, gibt den rechten Subbaum zurück.
        if(!right) return left; // Wenn der rechte Subbaum null ist, gibt den linken Subbaum zurück.
        // clang-format on

        Node* result;

        // Entscheidet, welcher Subbaum an der Wurzel basierend auf den Prioritäten bleibt.
        if(left->prio > right->prio) {
            left->right = join(left->right, right);
            result = left;
        } else {
            right->left = join(left, right->left);
            result = right;
        }

        // Aktualisiert die Größe des resultierenden Baums.
        update_size(result);

        return result;
    }

    // Rekursiv zerstört (löscht) einen Baum ab einem gegebenen Knoten.
    constexpr static auto destroy(Node* node) noexcept -> void
    {
        if(node) {
            destroy(node->left);
            destroy(node->right);
            delete node; // Löscht den aktuellen Knoten.
        }
    }

    // Hilfsfunktion, um eine tiefe Kopie des Baumes zu erstellen.
    constexpr static auto copy(Node* node) noexcept -> Node*
    {
        // clang-format off
        if(!node) return nullptr; // Wenn der gegebene Knoten null ist, gibt null zurück.
        // clang-format on

        Node* newNode = new Node;
        newNode->elem = node->elem;         // Kopiert den Schlüsselwert.
        newNode->prio = node->prio;         // Kopiert die Priorität.
        newNode->left = copy(node->left);   // Rekursiv kopiert den linken Subbaum.
        newNode->right = copy(node->right); // Rekursiv kopiert den rechten Subbaum.
        newNode->size = node->size;         // Kopiert die Größe.
        return newNode;
    }

private:
    Node* root_; // Zeiger auf den Wurzelknoten des Treaps.
};

} // namespace cvm
