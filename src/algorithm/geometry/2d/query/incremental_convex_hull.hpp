#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_INCREMENTAL_CONVEX_HULL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_INCREMENTAL_CONVEX_HULL_HPP_INCLUDED

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "../predicate/integer_orientation.hpp"

template<std::integral Coordinate>
class IncrementalConvexHull{
public:
    using PointType = IntegerPoint<Coordinate>;
    using WideInteger = GeometryIntegerWide;

private:
    struct Chain{
        struct Node{
            PointType point;
            std::uint64_t priority;
            std::unique_ptr<Node> left;
            std::unique_ptr<Node> right;
            std::size_t size = 1;
            WideInteger edge_sum = 0;

            Node(const PointType& point_, std::uint64_t priority_)
                : point(point_), priority(priority_){}
        };

        std::unique_ptr<Node> root;
        bool upper;
        std::uint64_t random_state;

        explicit Chain(bool upper_, std::uint64_t seed)
            : upper(upper_), random_state(seed){}

        static std::size_t node_size(const std::unique_ptr<Node>& node){
            return node == nullptr ? 0 : node->size;
        }

        static const PointType& first_point(const Node* node){
            while(node->left != nullptr) node = node->left.get();
            return node->point;
        }

        static const PointType& last_point(const Node* node){
            while(node->right != nullptr) node = node->right.get();
            return node->point;
        }

        static WideInteger origin_cross(
            const PointType& first,
            const PointType& second
        ){
            return WideInteger(static_cast<__int128_t>(first.x))
                    * WideInteger(static_cast<__int128_t>(second.y))
                - WideInteger(static_cast<__int128_t>(first.y))
                    * WideInteger(static_cast<__int128_t>(second.x));
        }

        static void pull(Node* node){
            node->size = 1 + node_size(node->left) + node_size(node->right);
            node->edge_sum = 0;
            if(node->left != nullptr){
                node->edge_sum += node->left->edge_sum;
                node->edge_sum += origin_cross(
                    last_point(node->left.get()), node->point
                );
            }
            if(node->right != nullptr){
                node->edge_sum += origin_cross(
                    node->point, first_point(node->right.get())
                );
                node->edge_sum += node->right->edge_sum;
            }
        }

        std::uint64_t next_random(){
            random_state += 0x9e3779b97f4a7c15ULL;
            std::uint64_t value = random_state;
            value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
            value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
            return value ^ (value >> 31);
        }

        static void split(
            std::unique_ptr<Node> node,
            const Coordinate& key,
            std::unique_ptr<Node>& left,
            std::unique_ptr<Node>& right
        ){
            if(node == nullptr){
                left.reset();
                right.reset();
                return;
            }
            if(node->point.x < key){
                split(std::move(node->right), key, node->right, right);
                pull(node.get());
                left = std::move(node);
            }else{
                split(std::move(node->left), key, left, node->left);
                pull(node.get());
                right = std::move(node);
            }
        }

        static void insert_node(
            std::unique_ptr<Node>& node,
            std::unique_ptr<Node> inserted
        ){
            if(node == nullptr){
                node = std::move(inserted);
                return;
            }
            if(inserted->priority > node->priority){
                split(
                    std::move(node), inserted->point.x,
                    inserted->left, inserted->right
                );
                pull(inserted.get());
                node = std::move(inserted);
            }else if(inserted->point.x < node->point.x){
                insert_node(node->left, std::move(inserted));
                pull(node.get());
            }else{
                insert_node(node->right, std::move(inserted));
                pull(node.get());
            }
        }

        static std::unique_ptr<Node> merge(
            std::unique_ptr<Node> left,
            std::unique_ptr<Node> right
        ){
            if(left == nullptr) return right;
            if(right == nullptr) return left;
            if(left->priority > right->priority){
                left->right = merge(std::move(left->right), std::move(right));
                pull(left.get());
                return left;
            }
            right->left = merge(std::move(left), std::move(right->left));
            pull(right.get());
            return right;
        }

        static void erase_node(
            std::unique_ptr<Node>& node,
            const Coordinate& key
        ){
            if(node->point.x == key){
                node = merge(std::move(node->left), std::move(node->right));
                return;
            }
            if(key < node->point.x) erase_node(node->left, key);
            else erase_node(node->right, key);
            pull(node.get());
        }

        const Node* find_node(const Coordinate& key) const{
            const Node* node = root.get();
            while(node != nullptr){
                if(key == node->point.x) return node;
                node = key < node->point.x
                    ? node->left.get() : node->right.get();
            }
            return nullptr;
        }

        const Node* lower_bound_node(const Coordinate& key) const{
            const Node* node = root.get();
            const Node* answer = nullptr;
            while(node != nullptr){
                if(!(node->point.x < key)){
                    answer = node;
                    node = node->left.get();
                }else{
                    node = node->right.get();
                }
            }
            return answer;
        }

        const Node* predecessor_node(const Coordinate& key) const{
            const Node* node = root.get();
            const Node* answer = nullptr;
            while(node != nullptr){
                if(node->point.x < key){
                    answer = node;
                    node = node->right.get();
                }else{
                    node = node->left.get();
                }
            }
            return answer;
        }

        const Node* successor_node(const Coordinate& key) const{
            const Node* node = root.get();
            const Node* answer = nullptr;
            while(node != nullptr){
                if(key < node->point.x){
                    answer = node;
                    node = node->left.get();
                }else{
                    node = node->right.get();
                }
            }
            return answer;
        }

        bool strict_turn(
            const PointType& first,
            const PointType& middle,
            const PointType& last
        ) const{
            const int orientation = integer_orientation(first, middle, last);
            return upper ? orientation < 0 : orientation > 0;
        }

        static WideInteger dot_value(
            const PointType& point,
            const PointType& direction
        ){
            return WideInteger(static_cast<__int128_t>(point.x))
                    * WideInteger(static_cast<__int128_t>(direction.x))
                + WideInteger(static_cast<__int128_t>(point.y))
                    * WideInteger(static_cast<__int128_t>(direction.y));
        }

        const Node* peak_node(const PointType& direction) const{
            const Node* node = root.get();
            while(node != nullptr){
                const WideInteger current = dot_value(node->point, direction);
                if(node->right != nullptr){
                    const Node* next = node->right.get();
                    while(next->left != nullptr) next = next->left.get();
                    if(dot_value(next->point, direction) > current){
                        node = node->right.get();
                        continue;
                    }
                }
                if(node->left != nullptr){
                    const Node* previous = node->left.get();
                    while(previous->right != nullptr){
                        previous = previous->right.get();
                    }
                    if(dot_value(previous->point, direction) > current){
                        node = node->left.get();
                        continue;
                    }
                }
                return node;
            }
            return nullptr;
        }

        static void append_in_order(
            const Node* node,
            std::vector<PointType>& result
        ){
            if(node == nullptr) return;
            append_in_order(node->left.get(), result);
            result.push_back(node->point);
            append_in_order(node->right.get(), result);
        }

    public:
        bool add(const PointType& point){
            const Node* same_x = find_node(point.x);
            if(same_x != nullptr){
                const bool farther = upper
                    ? same_x->point.y < point.y
                    : point.y < same_x->point.y;
                if(!farther) return false;
                erase_node(root, point.x);
            }

            const Node* previous = predecessor_node(point.x);
            const Node* next = successor_node(point.x);
            if(previous != nullptr && next != nullptr &&
                !strict_turn(previous->point, point, next->point)){
                return false;
            }

            insert_node(
                root, std::make_unique<Node>(point, next_random())
            );

            previous = predecessor_node(point.x);
            while(previous != nullptr){
                const Node* before = predecessor_node(previous->point.x);
                if(before == nullptr ||
                    strict_turn(before->point, previous->point, point)){
                    break;
                }
                const Coordinate erased_x = previous->point.x;
                erase_node(root, erased_x);
                previous = predecessor_node(point.x);
            }

            next = successor_node(point.x);
            while(next != nullptr){
                const Node* after = successor_node(next->point.x);
                if(after == nullptr ||
                    strict_turn(point, next->point, after->point)){
                    break;
                }
                const Coordinate erased_x = next->point.x;
                erase_node(root, erased_x);
                next = successor_node(point.x);
            }
            return true;
        }

        std::size_t size() const{
            return node_size(root);
        }

        const PointType& first() const{
            return first_point(root.get());
        }

        const PointType& last() const{
            return last_point(root.get());
        }

        const WideInteger& edge_sum() const{
            static const WideInteger zero = 0;
            return root == nullptr ? zero : root->edge_sum;
        }

        int boundary_side(const PointType& point) const{
            const Node* at_x = find_node(point.x);
            if(at_x != nullptr){
                return (point.y > at_x->point.y)
                    - (point.y < at_x->point.y);
            }
            const Node* next = lower_bound_node(point.x);
            if(next == nullptr) return 1;
            const Node* previous = predecessor_node(point.x);
            if(previous == nullptr) return -1;
            return integer_orientation(previous->point, next->point, point);
        }

        PointType support(const PointType& direction) const{
            const Node* best = peak_node(direction);
            PointType result = best->point;
            const Node* previous = predecessor_node(result.x);
            const Node* next = successor_node(result.x);
            for(const Node* candidate: {previous, next}){
                if(candidate == nullptr) continue;
                const WideInteger comparison =
                    dot_value(candidate->point, direction)
                    - dot_value(result, direction);
                if(comparison > 0 ||
                    (comparison == 0 && candidate->point < result)){
                    result = candidate->point;
                }
            }
            return result;
        }

        std::vector<PointType> points_in_order() const{
            std::vector<PointType> result;
            result.reserve(size());
            append_in_order(root.get(), result);
            return result;
        }
    };

    Chain lower_;
    Chain upper_;

    static WideInteger origin_cross(
        const PointType& first,
        const PointType& second
    ){
        return WideInteger(static_cast<__int128_t>(first.x))
                * WideInteger(static_cast<__int128_t>(second.y))
            - WideInteger(static_cast<__int128_t>(first.y))
                * WideInteger(static_cast<__int128_t>(second.x));
    }

    static WideInteger dot_value(
        const PointType& point,
        const PointType& direction
    ){
        return WideInteger(static_cast<__int128_t>(point.x))
                * WideInteger(static_cast<__int128_t>(direction.x))
            + WideInteger(static_cast<__int128_t>(point.y))
                * WideInteger(static_cast<__int128_t>(direction.y));
    }

public:
    IncrementalConvexHull()
        : lower_(false, 0x243f6a8885a308d3ULL),
          upper_(true, 0x13198a2e03707344ULL){}

    bool insert(const PointType& point){
        const bool lower_changed = lower_.add(point);
        const bool upper_changed = upper_.add(point);
        return lower_changed || upper_changed;
    }

    bool empty() const{
        return lower_.size() == 0;
    }

    std::size_t size() const{
        if(empty()) return 0;
        std::size_t result = lower_.size() + upper_.size();
        const bool same_x = lower_.first().x == lower_.last().x;
        if(lower_.first() == upper_.first()) --result;
        if(!same_x && lower_.last() == upper_.last()) --result;
        return result;
    }

    WideInteger doubled_area() const{
        if(empty()) return 0;
        WideInteger result = lower_.edge_sum() - upper_.edge_sum();
        result += origin_cross(lower_.last(), upper_.last());
        result += origin_cross(upper_.first(), lower_.first());
        return result.absolute();
    }

    int contains(const PointType& point) const{
        if(empty()) return 0;
        if(point.x < lower_.first().x || lower_.last().x < point.x) return 0;

        const int lower_side = lower_.boundary_side(point);
        const int upper_side = upper_.boundary_side(point);
        if(lower_side < 0 || upper_side > 0) return 0;
        if(lower_side == 0 || upper_side == 0) return 1;
        if(point.x == lower_.first().x || point.x == lower_.last().x){
            return 1;
        }
        return 2;
    }

    std::optional<PointType> support(const PointType& direction) const{
        if(empty()) return std::nullopt;
        PointType result;
        if(direction.y > 0){
            result = upper_.support(direction);
        }else if(direction.y < 0){
            result = lower_.support(direction);
        }else{
            std::array<PointType, 4> candidates{
                lower_.first(), lower_.last(), upper_.first(), upper_.last()
            };
            result = candidates[0];
            for(const PointType& candidate: candidates){
                const WideInteger comparison = dot_value(candidate, direction)
                    - dot_value(result, direction);
                if(comparison > 0 ||
                    (comparison == 0 && candidate < result)){
                    result = candidate;
                }
            }
        }
        return result;
    }

    std::vector<PointType> vertices() const{
        if(empty()) return {};
        std::vector<PointType> result = lower_.points_in_order();
        std::vector<PointType> upper = upper_.points_in_order();
        for(auto iterator = upper.rbegin(); iterator != upper.rend(); ++iterator){
            if(!result.empty() && *iterator == result.back()) continue;
            if(iterator + 1 == upper.rend() && !result.empty() &&
                *iterator == result.front()){
                continue;
            }
            result.push_back(*iterator);
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_QUERY_INCREMENTAL_CONVEX_HULL_HPP_INCLUDED
