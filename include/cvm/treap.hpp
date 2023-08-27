#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <random>
#include <type_traits>

namespace cvm {

template<class K, class P = double>
class Treap
{
public:
    constexpr Treap() noexcept
        : root_(nullptr)
    {
    }

    constexpr Treap(const Treap& other) noexcept
        : root_(nullptr)
    {
        root_ = copy(other.root_);
    }

    constexpr auto operator=(const Treap& other) noexcept -> Treap&
    {
        // clang-format off
        if(&other == this) return *this;
        // clang-format on

        destroy(root_);
        root_ = copy(other.root_);
        return *this;
    }

    constexpr Treap(Treap&& other) noexcept
        : root_(other.root_)
    {
        other.root_ = nullptr;
    }

    auto operator=(Treap&& other) noexcept -> Treap&
    {
        // clang-format off
        if(&other == this) return *this;
        // clang-format on

        destroy(root_);
        root_ = other.root_;
        other.root_ = nullptr;
        return *this;
    }

    ~Treap() noexcept
    {
        destroy(root_);
    }

    auto insert(const K& elem) noexcept -> void
    {
        const auto prio = generate_prio();
        insert(elem, prio);
    }

    auto insert(K elem, P prio) noexcept -> void
    {
        // This is a helper lambda to perform the recursive insert
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

            // Update size
            update_size(node);

            return node;
        };

        root_ = insert_recursive(insert_recursive, root_, std::move(elem), prio);
    }

    [[nodiscard]] auto contains(const K& elem) const noexcept -> bool
    {
        // This is a helper lambda to perform the recursive search
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

    auto delete_elem(const K& elem) noexcept
    {
        // This is a helper lambda to perform the recursive deletion
        constexpr static auto delete_recursive =
            [&](auto& self, auto* node, const K& elem) -> Node* {
            // Base case: node not found, return null.
            // clang-format off
            if(!node) return nullptr;
            // clang-format on

            // If node to be deleted is found.
            if(elem == node->elem) {

                // If the node has both children, decide the rotation based on priority.
                if(node->left && node->right) {
                    if(node->left->prio > node->right->prio) {
                        // Rotate right
                        node = rotate_right(node);
                        node->right = self(self, node->right, elem);
                    } else {
                        // Rotate left
                        node = rotate_left(node);
                        node->left = self(self, node->left, elem);
                    }
                } else {
                    // Node with only one child or no child.
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
                // If the element to delete is less than the current node's element.
                node->left = self(self, node->left, elem);
            } else {
                // If the element to delete is greater than the current node's element.
                node->right = self(self, node->right, elem);
            }

            // Update size.
            update_size(node);

            return node;
        };

        root_ = delete_recursive(delete_recursive, root_, elem);
    }

    constexpr auto pop() noexcept -> std::optional<std::pair<K, P>>
    {
        // Check if root is empty
        // clang-format off
        if(!root_) return std::nullopt;
        // clang-format on

        // Get the root value
        auto root_value = std::move(root_->elem);
        auto prio = root_->prio;

        // Save pointers to left and right children
        auto* const leftChild = root_->left;
        auto* const rightChild = root_->right;

        // Delete the root
        delete root_;

        // create new treap from left and right subtree
        root_ = join(leftChild, rightChild);

        return std::pair{std::move(root_value), prio};
    }

    [[nodiscard]] constexpr auto top() const noexcept -> std::optional<std::pair<K, P>>
    {
        // Check if root is empty
        // clang-format off
        if(!root_) return std::nullopt;
        // clang-format on

        return std::pair{root_->elem, root_->prio};
    }

    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t
    {
        return root_ ? root_->size : 0;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return not static_cast<bool>(root_);
    }

    constexpr auto clear() noexcept -> void
    {
        destroy(root_);
        root_ = nullptr;
    }

    [[nodiscard]] static auto generate_prio() noexcept -> P
    {
        static std::random_device rd{};
        static std::mt19937 gen = std::mt19937{rd()};
        static std::conditional_t<std::is_floating_point_v<P>,
                                  std::uniform_real_distribution<>,
                                  std::uniform_int_distribution<>>
            dist{};

        return dist(gen);
    }

private:
    struct Node
    {
        K elem;
        P prio;
        Node* left = nullptr;
        Node* right = nullptr;
        std::size_t size = 1;
    };

    constexpr static auto update_size(Node* x) noexcept
    {
        x->size = 1 + (x->left ? x->left->size : 0) + (x->right ? x->right->size : 0);
    }

    constexpr static auto rotate_left(Node* x) noexcept -> Node*
    {
        auto* const y = x->right;
        x->right = y->left;
        y->left = x;

        // Update sizes
        update_size(x);
        update_size(y);

        return y;
    }

    constexpr static auto rotate_right(Node* y) noexcept -> Node*
    {
        auto* const x = y->left;
        y->left = x->right;
        x->right = y;

        // Update sizes
        update_size(x);
        update_size(y);

        return x;
    }

    constexpr static auto join(Node* left, Node* right) noexcept -> Node*
    {
        // clang-format off
        if(!left)  return right;
        if(!right) return left;
        // clang-format on

        Node* result;
        if(left->prio > right->prio) {
            left->right = join(left->right, right);
            result = left;
        } else {
            right->left = join(left, right->left);
            result = right;
        }

        // Update size:
        update_size(result);

        return result;
    }

    constexpr static auto destroy(Node* node) noexcept -> void
    {
        if(node) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    // Utility function to create a deep copy of the tree
    constexpr static auto copy(Node* node) noexcept -> Node*
    {
        // clang-format off
        if(!node) return nullptr;
        // clang-format on

        Node* newNode = new Node;
        newNode->elem = node->elem;
        newNode->prio = node->prio;
        newNode->left = copy(node->left);
        newNode->right = copy(node->right);
        newNode->size = node->size;
        return newNode;
    }

private:
    Node* root_;
};

} // namespace cvm
