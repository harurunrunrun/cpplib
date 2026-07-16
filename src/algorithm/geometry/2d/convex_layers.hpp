#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

namespace convex_layers_detail{

struct Point{
    __int128_t x;
    __int128_t y;
};

inline __int128_t cross(const Point& first, const Point& second, const Point& third){
    return (second.x - first.x) * (third.y - first.y)
        - (second.y - first.y) * (third.x - first.x);
}


// A decremental upper hull specialized for peeling.  Every point is stored in
// exactly one chain.  A deletion only splices chains and moves links down the
// fixed tree, so each link is scanned O(1) times per tree level.
class AmortizedDecrementalUpperHull{
    struct Link{
        Point point{};
        Link* previous = nullptr;
        Link* next = nullptr;
        int index = -1;
    };

    struct Node{
        Link* chain = nullptr;
        Link* chain_back = nullptr;
        Link* tangent = nullptr;
        int left_child = -1;
        int right_child = -1;
    };

public:
    explicit AmortizedDecrementalUpperHull(const std::vector<Point>& points)
        : size_(points.size()), remaining_(points.size()), erased_(points.size(), false),
          nodes_(2 * points.size()){
        assert(std::is_sorted(points.begin(), points.end(), [](const Point& left, const Point& right){
            return left.x < right.x || (left.x == right.x && left.y < right.y);
        }));
        if(points.empty()) return;
        links_.reserve(points.size());
        for(std::size_t index = 0; index < points.size(); ++index){
            links_.push_back({points[index], nullptr, nullptr, static_cast<int>(index)});
        }
        for(std::size_t index = 0; index < points.size(); ++index){
            nodes_[index + size_] = {&links_[index], &links_[index], nullptr, -1, -1};
        }
        if(points.size() == 1) nodes_[0] = nodes_[1];
        std::size_t next_node = 0;
        build(next_node, 0, static_cast<int>(points.size()));
    }

    bool empty() const{
        return remaining_ == 0;
    }

    std::vector<int> hull_indices() const{
        std::vector<int> result;
        if(nodes_.empty()) return result;
        for(Link* link = nodes_[0].chain; link != nullptr; link = link->next){
            result.push_back(link->index);
        }
        return result;
    }

    bool erase(int index){
        assert(0 <= index && static_cast<std::size_t>(index) < size_);
        if(erased_[static_cast<std::size_t>(index)]) return false;
        erased_[static_cast<std::size_t>(index)] = true;
        --remaining_;
        if(nodes_[0].chain == nodes_[0].chain_back){
            nodes_[0].chain = nullptr;
            nodes_[0].chain_back = nullptr;
        }else{
            erase_from_node(0, 0, static_cast<int>(size_), index);
        }
        return true;
    }

private:
    static Point subtract(const Point& first, const Point& second){
        return {first.x - second.x, first.y - second.y};
    }

    static int orientation(const Point& first, const Point& second){
        const __int128_t value = first.x * second.y - first.y * second.x;
        return (value > 0) - (value < 0);
    }

    static int orientation(const Point& first, const Point& second, const Point& third){
        const __int128_t value = cross(first, second, third);
        return (value > 0) - (value < 0);
    }

    std::pair<Link*, Link*> find_bridge(Link* left, Link* right) const{
        while(left->next != nullptr || right->next != nullptr){
            if(right->next == nullptr || (left->next != nullptr
                && orientation(subtract(left->next->point, left->point),
                               subtract(right->next->point, right->point)) <= 0)){
                if(orientation(left->point, left->next->point, right->point) <= 0){
                    left = left->next;
                }else{
                    break;
                }
            }else if(orientation(left->point, right->point, right->next->point) > 0){
                right = right->next;
            }else{
                break;
            }
        }
        return {left, right};
    }

    std::pair<Link*, Link*> find_bridge_reversed(Link* left, Link* right) const{
        while(right->previous != nullptr || left->previous != nullptr){
            if(left->previous == nullptr || (right->previous != nullptr
                && orientation(subtract(right->previous->point, right->point),
                               subtract(left->previous->point, left->point)) >= 0)){
                if(orientation(right->point, right->previous->point, left->point) >= 0){
                    right = right->previous;
                }else{
                    break;
                }
            }else if(orientation(right->point, left->point, left->previous->point) < 0){
                left = left->previous;
            }else{
                break;
            }
        }
        return {right, left};
    }

    template<bool reversed>
    void fix_chain(int node, Link* left, Link* right){
        if constexpr(reversed){
            const auto bridge = find_bridge_reversed(left, right);
            right = bridge.first;
            left = bridge.second;
        }else{
            const auto bridge = find_bridge(left, right);
            left = bridge.first;
            right = bridge.second;
        }
        Node& current = nodes_[static_cast<std::size_t>(node)];
        Node& left_node = nodes_[static_cast<std::size_t>(current.left_child)];
        Node& right_node = nodes_[static_cast<std::size_t>(current.right_child)];
        current.tangent = left;
        current.chain = left_node.chain;
        current.chain_back = right_node.chain_back;
        left_node.chain = left->next;
        right_node.chain_back = right->previous;
        if(left->next != nullptr) left->next->previous = nullptr;
        else left_node.chain_back = nullptr;
        if(right->previous != nullptr) right->previous->next = nullptr;
        else right_node.chain = nullptr;
        left->next = right;
        right->previous = left;
    }

    void take_chain(int destination, int source){
        nodes_[static_cast<std::size_t>(destination)].chain =
            nodes_[static_cast<std::size_t>(source)].chain;
        nodes_[static_cast<std::size_t>(source)].chain = nullptr;
        nodes_[static_cast<std::size_t>(destination)].chain_back =
            nodes_[static_cast<std::size_t>(source)].chain_back;
        nodes_[static_cast<std::size_t>(source)].chain_back = nullptr;
    }

    void erase_from_node(int node, int range_left, int range_right, int index){
        Node& current = nodes_[static_cast<std::size_t>(node)];
        const int middle = (range_left + range_right) / 2;
        const int child = index < middle ? current.left_child : current.right_child;
        if(current.tangent == nullptr){
            nodes_[static_cast<std::size_t>(child)].chain = current.chain;
            nodes_[static_cast<std::size_t>(child)].chain_back = current.chain_back;
            if(index < middle) erase_from_node(child, range_left, middle, index);
            else erase_from_node(child, middle, range_right, index);
            take_chain(node, child);
            return;
        }

        Link* left = current.tangent;
        Link* right = left->next;
        Node& left_node = nodes_[static_cast<std::size_t>(current.left_child)];
        Node& right_node = nodes_[static_cast<std::size_t>(current.right_child)];
        left->next = left_node.chain;
        if(left_node.chain != nullptr) left_node.chain->previous = left;
        else left_node.chain_back = left;
        left_node.chain = current.chain;
        right->previous = right_node.chain_back;
        if(right_node.chain_back != nullptr) right_node.chain_back->next = right;
        else right_node.chain = right;
        right_node.chain_back = current.chain_back;

        Node& child_node = nodes_[static_cast<std::size_t>(child)];
        assert(child_node.chain != nullptr);
        if(child_node.chain == child_node.chain_back && child_node.chain->index == index){
            child_node.chain = nullptr;
            child_node.chain_back = nullptr;
            take_chain(node, index < middle ? current.right_child : current.left_child);
            current.tangent = nullptr;
        }else if(index < middle){
            if(left->index == index) left = left->next;
            erase_from_node(child, range_left, middle, index);
            if(left == nullptr) left = left_node.chain_back;
            fix_chain<true>(node, left, right);
        }else{
            if(right->index == index) right = right->previous;
            erase_from_node(child, middle, range_right, index);
            if(right == nullptr) right = right_node.chain;
            fix_chain<false>(node, left, right);
        }
    }

    std::size_t build(std::size_t& next_node, int left, int right){
        if(right - left == 1) return static_cast<std::size_t>(left) + size_;
        const int middle = (left + right) / 2;
        const std::size_t node = next_node++;
        nodes_[node].left_child = static_cast<int>(build(next_node, left, middle));
        nodes_[node].right_child = static_cast<int>(build(next_node, middle, right));
        fix_chain<false>(static_cast<int>(node),
            nodes_[static_cast<std::size_t>(nodes_[node].left_child)].chain,
            nodes_[static_cast<std::size_t>(nodes_[node].right_child)].chain);
        return node;
    }

    std::size_t size_;
    std::size_t remaining_;
    std::vector<bool> erased_;
    std::vector<Node> nodes_;
    std::vector<Link> links_;
};

}  // namespace convex_layers_detail

template<std::integral Coordinate>
std::vector<int> convex_layers(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
){
    if(points.empty()) return {};

    std::vector<std::size_t> order(points.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        if(points[left].first != points[right].first){
            return points[left].first < points[right].first;
        }
        if(points[left].second != points[right].second){
            return points[left].second < points[right].second;
        }
        return left < right;
    });

    std::vector<convex_layers_detail::Point> unique_points;
    std::vector<std::vector<std::size_t>> original_indices;
    for(std::size_t index: order){
        if(unique_points.empty()
            || unique_points.back().x != static_cast<__int128_t>(points[index].first)
            || unique_points.back().y != static_cast<__int128_t>(points[index].second)){
            unique_points.push_back({
                static_cast<__int128_t>(points[index].first),
                static_cast<__int128_t>(points[index].second)
            });
            original_indices.emplace_back();
        }
        original_indices.back().push_back(index);
    }

    convex_layers_detail::AmortizedDecrementalUpperHull upper_hull(unique_points);
    std::vector<convex_layers_detail::Point> reflected;
    reflected.reserve(unique_points.size());
    for(std::size_t index = unique_points.size(); index-- > 0;){
        reflected.push_back({-unique_points[index].x, -unique_points[index].y});
    }
    convex_layers_detail::AmortizedDecrementalUpperHull lower_hull(reflected);

    std::vector<int> unique_answer(unique_points.size(), 0);
    std::size_t removed = 0;
    for(int layer = 1; removed < unique_points.size(); ++layer){
        std::vector<int> boundary = upper_hull.hull_indices();
        const std::vector<int> reflected_boundary = lower_hull.hull_indices();
        boundary.reserve(boundary.size() + reflected_boundary.size());
        for(int index: reflected_boundary){
            boundary.push_back(static_cast<int>(unique_points.size()) - 1 - index);
        }
        std::sort(boundary.begin(), boundary.end());
        boundary.erase(std::unique(boundary.begin(), boundary.end()), boundary.end());
        assert(!boundary.empty());
        for(int index: boundary){
            unique_answer[static_cast<std::size_t>(index)] = layer;
            upper_hull.erase(index);
            lower_hull.erase(static_cast<int>(unique_points.size()) - 1 - index);
        }
        removed += boundary.size();
    }

    std::vector<int> answer(points.size());
    for(std::size_t index = 0; index < unique_points.size(); ++index){
        for(std::size_t original: original_indices[index]){
            answer[original] = unique_answer[index];
        }
    }
    return answer;
}
