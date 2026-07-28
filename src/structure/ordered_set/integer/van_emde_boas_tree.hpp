#ifndef CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_VAN_EMDE_BOAS_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_VAN_EMDE_BOAS_TREE_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>

template<int BITS>
class VanEmdeBoasTree {
    static_assert(1 <= BITS && BITS <= 63);
    class Node {
        int bits_;
        int low_bits_;
        std::uint64_t minimum_ = 0;
        std::uint64_t maximum_ = 0;
        bool empty_ = true;
        std::uint64_t mask_ = 0;
        std::unique_ptr<Node> summary_;
        std::unordered_map<std::uint64_t, std::unique_ptr<Node>> clusters_;

        [[nodiscard]] bool small() const noexcept { return bits_ <= 6; }
        [[nodiscard]] std::uint64_t high(std::uint64_t value) const {
            return value >> low_bits_;
        }
        [[nodiscard]] std::uint64_t low(std::uint64_t value) const {
            return value & ((std::uint64_t{1} << low_bits_) - 1);
        }
        [[nodiscard]] std::uint64_t combine(
            std::uint64_t high_part, std::uint64_t low_part
        ) const {
            return (high_part << low_bits_) | low_part;
        }

    public:
        explicit Node(int bits)
            : bits_(bits), low_bits_(bits / 2) {}

        [[nodiscard]] bool empty() const noexcept { return empty_; }
        [[nodiscard]] std::uint64_t minimum() const { return minimum_; }
        [[nodiscard]] std::uint64_t maximum() const { return maximum_; }

        [[nodiscard]] bool contains(std::uint64_t value) const {
            if (empty_) return false;
            if (small()) return (mask_ >> value) & 1U;
            if (value == minimum_ || value == maximum_) return true;
            const auto iterator = clusters_.find(high(value));
            return iterator != clusters_.end()
                && iterator->second->contains(low(value));
        }

        void insert(std::uint64_t value) {
            if (small()) {
                const std::uint64_t bit = std::uint64_t{1} << value;
                mask_ |= bit;
                empty_ = false;
                minimum_ = static_cast<std::uint64_t>(
                    __builtin_ctzll(mask_)
                );
                maximum_ = 63U - static_cast<std::uint64_t>(
                    __builtin_clzll(mask_)
                );
                return;
            }
            if (empty_) {
                minimum_ = maximum_ = value;
                empty_ = false;
                return;
            }
            if (value == minimum_ || value == maximum_) return;
            if (value < minimum_) std::swap(value, minimum_);
            const std::uint64_t cluster_index = high(value);
            const std::uint64_t offset = low(value);
            auto& cluster = clusters_[cluster_index];
            if (!cluster) cluster = std::make_unique<Node>(low_bits_);
            if (cluster->empty()) {
                if (!summary_) {
                    summary_ = std::make_unique<Node>(bits_ - low_bits_);
                }
                summary_->insert(cluster_index);
            }
            cluster->insert(offset);
            if (value > maximum_) maximum_ = value;
        }

        void erase(std::uint64_t value) {
            if (empty_) return;
            if (small()) {
                mask_ &= ~(std::uint64_t{1} << value);
                if (mask_ == 0) {
                    empty_ = true;
                } else {
                    minimum_ = static_cast<std::uint64_t>(
                        __builtin_ctzll(mask_)
                    );
                    maximum_ = 63U - static_cast<std::uint64_t>(
                        __builtin_clzll(mask_)
                    );
                }
                return;
            }
            if (minimum_ == maximum_) {
                if (value == minimum_) empty_ = true;
                return;
            }
            if (value == minimum_) {
                const std::uint64_t first_cluster = summary_->minimum();
                value = combine(
                    first_cluster, clusters_[first_cluster]->minimum()
                );
                minimum_ = value;
            }
            const std::uint64_t cluster_index = high(value);
            auto iterator = clusters_.find(cluster_index);
            if (iterator == clusters_.end()) return;
            iterator->second->erase(low(value));
            if (iterator->second->empty()) {
                summary_->erase(cluster_index);
                clusters_.erase(iterator);
                if (summary_->empty()) {
                    summary_.reset();
                    maximum_ = minimum_;
                } else if (value == maximum_) {
                    const std::uint64_t last_cluster = summary_->maximum();
                    maximum_ = combine(
                        last_cluster, clusters_[last_cluster]->maximum()
                    );
                }
            } else if (value == maximum_) {
                maximum_ = combine(
                    cluster_index, iterator->second->maximum()
                );
            }
        }

        [[nodiscard]] std::optional<std::uint64_t> successor(
            std::uint64_t value
        ) const {
            if (empty_ || value >= maximum_) return std::nullopt;
            if (value < minimum_) return minimum_;
            if (small()) {
                const std::uint64_t above =
                    mask_ & (~std::uint64_t{0} << (value + 1));
                if (above == 0) return std::nullopt;
                return static_cast<std::uint64_t>(__builtin_ctzll(above));
            }
            const std::uint64_t cluster_index = high(value);
            const std::uint64_t offset = low(value);
            const auto iterator = clusters_.find(cluster_index);
            if (iterator != clusters_.end()
                && !iterator->second->empty()
                && offset < iterator->second->maximum()) {
                return combine(
                    cluster_index,
                    *iterator->second->successor(offset)
                );
            }
            const auto next_cluster = summary_
                ? summary_->successor(cluster_index) : std::nullopt;
            if (!next_cluster) return std::nullopt;
            return combine(
                *next_cluster, clusters_.at(*next_cluster)->minimum()
            );
        }

        [[nodiscard]] std::optional<std::uint64_t> predecessor(
            std::uint64_t value
        ) const {
            if (empty_ || value <= minimum_) return std::nullopt;
            if (value > maximum_) return maximum_;
            if (small()) {
                const std::uint64_t below =
                    mask_ & ((std::uint64_t{1} << value) - 1);
                if (below == 0) return std::nullopt;
                return 63U - static_cast<std::uint64_t>(
                    __builtin_clzll(below)
                );
            }
            const std::uint64_t cluster_index = high(value);
            const std::uint64_t offset = low(value);
            const auto iterator = clusters_.find(cluster_index);
            if (iterator != clusters_.end()
                && !iterator->second->empty()
                && offset > iterator->second->minimum()) {
                return combine(
                    cluster_index,
                    *iterator->second->predecessor(offset)
                );
            }
            const auto previous_cluster = summary_
                ? summary_->predecessor(cluster_index) : std::nullopt;
            if (!previous_cluster) return minimum_;
            return combine(
                *previous_cluster, clusters_.at(*previous_cluster)->maximum()
            );
        }
    };

    Node root_{BITS};
    std::size_t size_ = 0;
    static constexpr std::uint64_t LIMIT =
        std::uint64_t{1} << BITS;

    static void check_key(std::uint64_t key) {
        if (key >= LIMIT) {
            throw std::out_of_range("VanEmdeBoasTree key outside universe");
        }
    }

public:
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool contains(std::uint64_t key) const {
        check_key(key);
        return root_.contains(key);
    }
    bool insert(std::uint64_t key) {
        check_key(key);
        if (root_.contains(key)) return false;
        root_.insert(key);
        ++size_;
        return true;
    }
    bool erase(std::uint64_t key) {
        check_key(key);
        if (!root_.contains(key)) return false;
        root_.erase(key);
        --size_;
        return true;
    }
    [[nodiscard]] std::uint64_t minimum() const {
        if (empty()) throw std::out_of_range("VanEmdeBoasTree::minimum");
        return root_.minimum();
    }
    [[nodiscard]] std::uint64_t maximum() const {
        if (empty()) throw std::out_of_range("VanEmdeBoasTree::maximum");
        return root_.maximum();
    }
    [[nodiscard]] std::optional<std::uint64_t> successor(
        std::uint64_t key
    ) const {
        check_key(key);
        return root_.successor(key);
    }
    [[nodiscard]] std::optional<std::uint64_t> predecessor(
        std::uint64_t key
    ) const {
        check_key(key);
        return root_.predecessor(key);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_VAN_EMDE_BOAS_TREE_HPP_INCLUDED
