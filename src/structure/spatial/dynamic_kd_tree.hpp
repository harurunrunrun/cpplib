#ifndef CPPLIB_SRC_STRUCTURE_SPATIAL_DYNAMIC_KD_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SPATIAL_DYNAMIC_KD_TREE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

template<class Coordinate, std::size_t DIMENSION>
class DynamicKdTree {
    static_assert(DIMENSION >= 1);
public:
    using Point = std::array<Coordinate, DIMENSION>;
    struct NearestResult {
        int id;
        long double squared_distance;
    };
private:
    struct Entry { Point point; int id; };
    struct Node {
        Entry entry;
        bool active = true;
        std::size_t size = 1;
        std::size_t active_size = 1;
        Point minimum;
        Point maximum;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        explicit Node(Entry entry_)
            : entry(std::move(entry_)),
              minimum(entry.point), maximum(entry.point) {}
    };
    std::unique_ptr<Node> root_;
    std::unordered_map<int, Point> points_;

    static std::size_t size_of(const std::unique_ptr<Node>& node) {
        return node ? node->size : 0;
    }
    static std::size_t active_of(const std::unique_ptr<Node>& node) {
        return node ? node->active_size : 0;
    }
    static void update(Node* node) {
        node->size = 1 + size_of(node->left) + size_of(node->right);
        node->active_size =
            static_cast<std::size_t>(node->active)
            + active_of(node->left) + active_of(node->right);
        node->minimum = node->entry.point;
        node->maximum = node->entry.point;
        for (std::size_t axis = 0; axis < DIMENSION; ++axis) {
            if (node->left) {
                node->minimum[axis] =
                    std::min(node->minimum[axis], node->left->minimum[axis]);
                node->maximum[axis] =
                    std::max(node->maximum[axis], node->left->maximum[axis]);
            }
            if (node->right) {
                node->minimum[axis] =
                    std::min(node->minimum[axis], node->right->minimum[axis]);
                node->maximum[axis] =
                    std::max(node->maximum[axis], node->right->maximum[axis]);
            }
        }
    }
    static void collect(const std::unique_ptr<Node>& node,
                        std::vector<Entry>& entries) {
        if (!node) return;
        collect(node->left, entries);
        if (node->active) entries.push_back(node->entry);
        collect(node->right, entries);
    }
    static std::unique_ptr<Node> build(
        std::vector<Entry>& entries, std::size_t left, std::size_t right,
        std::size_t depth
    ) {
        if (left == right) return nullptr;
        const std::size_t middle = left + (right - left) / 2;
        const std::size_t axis = depth % DIMENSION;
        std::nth_element(
            entries.begin() + static_cast<std::ptrdiff_t>(left),
            entries.begin() + static_cast<std::ptrdiff_t>(middle),
            entries.begin() + static_cast<std::ptrdiff_t>(right),
            [axis](const Entry& a, const Entry& b) {
                if (a.point[axis] != b.point[axis]) {
                    return a.point[axis] < b.point[axis];
                }
                return a.id < b.id;
            }
        );
        auto node = std::make_unique<Node>(std::move(entries[middle]));
        node->left = build(entries, left, middle, depth + 1);
        node->right = build(entries, middle + 1, right, depth + 1);
        update(node.get());
        return node;
    }
    static void rebuild(std::unique_ptr<Node>& node, std::size_t depth) {
        std::vector<Entry> entries;
        entries.reserve(active_of(node));
        collect(node, entries);
        node = build(entries, 0, entries.size(), depth);
    }
    static bool before(
        const Entry& a, const Entry& b, std::size_t axis
    ) {
        if (a.point[axis] != b.point[axis]) {
            return a.point[axis] < b.point[axis];
        }
        return a.id < b.id;
    }
    static void insert_impl(
        std::unique_ptr<Node>& node, Entry entry, std::size_t depth
    ) {
        if (!node) {
            node = std::make_unique<Node>(std::move(entry));
            return;
        }
        const std::size_t axis = depth % DIMENSION;
        if (before(entry, node->entry, axis)) {
            insert_impl(node->left, std::move(entry), depth + 1);
        } else {
            insert_impl(node->right, std::move(entry), depth + 1);
        }
        update(node.get());
        if (std::max(size_of(node->left), size_of(node->right)) * 4
            > node->size * 3) rebuild(node, depth);
    }
    static bool erase_impl(
        std::unique_ptr<Node>& node, const Entry& entry, std::size_t depth
    ) {
        if (!node) return false;
        bool erased = false;
        if (node->entry.id == entry.id && node->active) {
            node->active = false;
            erased = true;
        } else {
            const std::size_t axis = depth % DIMENSION;
            erased = before(entry, node->entry, axis)
                ? erase_impl(node->left, entry, depth + 1)
                : erase_impl(node->right, entry, depth + 1);
        }
        if (erased) {
            update(node.get());
            if (node->active_size * 2 < node->size) rebuild(node, depth);
        }
        return erased;
    }
    static bool box_intersects(
        const Node& node, const Point& lower, const Point& upper
    ) {
        for (std::size_t axis = 0; axis < DIMENSION; ++axis) {
            if (node.maximum[axis] < lower[axis]
                || upper[axis] < node.minimum[axis]) return false;
        }
        return true;
    }
    static bool point_inside(
        const Point& point, const Point& lower, const Point& upper
    ) {
        for (std::size_t axis = 0; axis < DIMENSION; ++axis) {
            if (point[axis] < lower[axis] || upper[axis] < point[axis]) {
                return false;
            }
        }
        return true;
    }
    static void range_impl(
        const Node* node, const Point& lower, const Point& upper,
        std::vector<int>& output
    ) {
        if (!node || !box_intersects(*node, lower, upper)) return;
        if (node->active && point_inside(node->entry.point, lower, upper)) {
            output.push_back(node->entry.id);
        }
        range_impl(node->left.get(), lower, upper, output);
        range_impl(node->right.get(), lower, upper, output);
    }
    static long double point_distance(const Point& a, const Point& b) {
        long double result = 0;
        for (std::size_t axis = 0; axis < DIMENSION; ++axis) {
            const long double difference =
                static_cast<long double>(a[axis])
                - static_cast<long double>(b[axis]);
            result += difference * difference;
        }
        return result;
    }
    static long double box_distance(const Node& node, const Point& point) {
        long double result = 0;
        for (std::size_t axis = 0; axis < DIMENSION; ++axis) {
            long double difference = 0;
            if (point[axis] < node.minimum[axis]) {
                difference = static_cast<long double>(node.minimum[axis])
                    - static_cast<long double>(point[axis]);
            } else if (node.maximum[axis] < point[axis]) {
                difference = static_cast<long double>(point[axis])
                    - static_cast<long double>(node.maximum[axis]);
            }
            result += difference * difference;
        }
        return result;
    }
    static void nearest_impl(
        const Node* node, const Point& point, NearestResult& best, bool& found
    ) {
        if (!node || (found && box_distance(*node, point)
            > best.squared_distance)) return;
        if (node->active) {
            const long double distance =
                point_distance(node->entry.point, point);
            if (!found || distance < best.squared_distance
                || (distance == best.squared_distance
                    && node->entry.id < best.id)) {
                best = {node->entry.id, distance};
                found = true;
            }
        }
        const Node* first = node->left.get();
        const Node* second = node->right.get();
        if (first && second
            && box_distance(*second, point) < box_distance(*first, point)) {
            std::swap(first, second);
        }
        nearest_impl(first, point, best, found);
        nearest_impl(second, point, best, found);
    }

public:
    [[nodiscard]] bool empty() const noexcept { return points_.empty(); }
    [[nodiscard]] std::size_t size() const noexcept { return points_.size(); }
    [[nodiscard]] bool contains(int id) const { return points_.contains(id); }
    bool insert(int id, const Point& point) {
        if (points_.contains(id)) return false;
        points_.emplace(id, point);
        insert_impl(root_, Entry{point, id}, 0);
        return true;
    }
    bool erase(int id) {
        auto iterator = points_.find(id);
        if (iterator == points_.end()) return false;
        const Entry entry{iterator->second, id};
        points_.erase(iterator);
        return erase_impl(root_, entry, 0);
    }
    [[nodiscard]] std::vector<int> range_search(
        const Point& lower, const Point& upper
    ) const {
        for (std::size_t axis = 0; axis < DIMENSION; ++axis) {
            if (upper[axis] < lower[axis]) {
                throw std::invalid_argument("DynamicKdTree invalid range");
            }
        }
        std::vector<int> result;
        range_impl(root_.get(), lower, upper, result);
        return result;
    }
    [[nodiscard]] std::optional<NearestResult> nearest(
        const Point& point
    ) const {
        NearestResult result{0, std::numeric_limits<long double>::infinity()};
        bool found = false;
        nearest_impl(root_.get(), point, result, found);
        return found ? std::optional<NearestResult>(result) : std::nullopt;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SPATIAL_DYNAMIC_KD_TREE_HPP_INCLUDED
