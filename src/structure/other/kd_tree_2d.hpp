#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct KdTreePoint2D{
    T x{};
    T y{};
    int id = -1;
};

struct KdTreeNearestResult2D{
    int id = -1;
    long double squared_distance =
        std::numeric_limits<long double>::infinity();
    bool found = false;

    bool exists() const{ return found; }
};

template<class T, int MAX_POINTS>
struct KdTree2D{
    static_assert(MAX_POINTS > 0);
    using Point = KdTreePoint2D<T>;

private:
    struct Node{
        Point point;
        int left = -1;
        int right = -1;
        T min_x{};
        T max_x{};
        T min_y{};
        T max_y{};
    };

    struct State{
        std::array<Point, MAX_POINTS> work;
        std::array<Node, MAX_POINTS> nodes;
    };

    int point_count = 0;
    int node_count = 0;
    int root_node = -1;
    std::unique_ptr<State> state;

    static bool less_value(const T& a, const T& b){
        return a < b;
    }

    static bool point_less(const Point& a, const Point& b, int axis){
        if(axis == 0){
            if(less_value(a.x, b.x)) return true;
            if(less_value(b.x, a.x)) return false;
            if(less_value(a.y, b.y)) return true;
            if(less_value(b.y, a.y)) return false;
        }else{
            if(less_value(a.y, b.y)) return true;
            if(less_value(b.y, a.y)) return false;
            if(less_value(a.x, b.x)) return true;
            if(less_value(b.x, a.x)) return false;
        }
        return a.id < b.id;
    }

    void include_child(Node& node, int child){
        if(child == -1) return;
        const auto& other = state->nodes[static_cast<std::size_t>(child)];
        if(less_value(other.min_x, node.min_x)) node.min_x = other.min_x;
        if(less_value(node.max_x, other.max_x)) node.max_x = other.max_x;
        if(less_value(other.min_y, node.min_y)) node.min_y = other.min_y;
        if(less_value(node.max_y, other.max_y)) node.max_y = other.max_y;
    }

    int build_rec(int left, int right, int depth){
        if(left == right) return -1;
        int middle = (left + right) >> 1;
        int axis = depth & 1;
        auto begin = state->work.begin();
        std::nth_element(
            begin + left,
            begin + middle,
            begin + right,
            [axis](const Point& a, const Point& b){ return point_less(a, b, axis); }
        );

        int node_id = node_count++;
        auto& node = state->nodes[static_cast<std::size_t>(node_id)];
        node.point = state->work[static_cast<std::size_t>(middle)];
        node.left = build_rec(left, middle, depth + 1);
        node.right = build_rec(middle + 1, right, depth + 1);
        node.min_x = node.max_x = node.point.x;
        node.min_y = node.max_y = node.point.y;
        include_child(node, node.left);
        include_child(node, node.right);
        return node_id;
    }

    bool disjoint(
        const Node& node,
        const T& min_x,
        const T& max_x,
        const T& min_y,
        const T& max_y
    ) const{
        return less_value(node.max_x, min_x) || less_value(max_x, node.min_x) ||
               less_value(node.max_y, min_y) || less_value(max_y, node.min_y);
    }

    bool covered(
        const Node& node,
        const T& min_x,
        const T& max_x,
        const T& min_y,
        const T& max_y
    ) const{
        return !less_value(node.min_x, min_x) && !less_value(max_x, node.max_x) &&
               !less_value(node.min_y, min_y) && !less_value(max_y, node.max_y);
    }

    bool contains(
        const Point& point,
        const T& min_x,
        const T& max_x,
        const T& min_y,
        const T& max_y
    ) const{
        return !less_value(point.x, min_x) && !less_value(max_x, point.x) &&
               !less_value(point.y, min_y) && !less_value(max_y, point.y);
    }

    void collect(int node_id, std::vector<int>& result) const{
        if(node_id == -1) return;
        const auto& node = state->nodes[static_cast<std::size_t>(node_id)];
        result.push_back(node.point.id);
        collect(node.left, result);
        collect(node.right, result);
    }

    void range_search_rec(
        int node_id,
        const T& min_x,
        const T& max_x,
        const T& min_y,
        const T& max_y,
        std::vector<int>& result
    ) const{
        if(node_id == -1) return;
        const auto& node = state->nodes[static_cast<std::size_t>(node_id)];
        if(disjoint(node, min_x, max_x, min_y, max_y)) return;
        if(covered(node, min_x, max_x, min_y, max_y)){
            collect(node_id, result);
            return;
        }
        if(contains(node.point, min_x, max_x, min_y, max_y)){
            result.push_back(node.point.id);
        }
        range_search_rec(node.left, min_x, max_x, min_y, max_y, result);
        range_search_rec(node.right, min_x, max_x, min_y, max_y, result);
    }

    static long double squared_difference(const T& first, const T& second){
        const long double difference = static_cast<long double>(first)
            - static_cast<long double>(second);
        return difference * difference;
    }

    static long double squared_distance(
        const Point& point,
        const T& x,
        const T& y
    ){
        return squared_difference(point.x, x) + squared_difference(point.y, y);
    }

    static long double box_squared_distance(
        const Node& node,
        const T& x,
        const T& y
    ){
        const long double query_x = static_cast<long double>(x);
        const long double query_y = static_cast<long double>(y);
        const long double minimum_x = static_cast<long double>(node.min_x);
        const long double maximum_x = static_cast<long double>(node.max_x);
        const long double minimum_y = static_cast<long double>(node.min_y);
        const long double maximum_y = static_cast<long double>(node.max_y);
        const long double dx = query_x < minimum_x
            ? minimum_x - query_x
            : (query_x > maximum_x ? query_x - maximum_x : 0.0L);
        const long double dy = query_y < minimum_y
            ? minimum_y - query_y
            : (query_y > maximum_y ? query_y - maximum_y : 0.0L);
        return dx * dx + dy * dy;
    }

    void nearest_neighbor_rec(
        int node_id,
        const T& x,
        const T& y,
        KdTreeNearestResult2D& result
    ) const{
        if(node_id == -1) return;
        const Node& node = state->nodes[static_cast<std::size_t>(node_id)];
        const long double current_distance = squared_distance(node.point, x, y);
        if(!result.exists() || current_distance < result.squared_distance
            || (current_distance == result.squared_distance
                && node.point.id < result.id)){
            result = {node.point.id, current_distance, true};
        }

        int first_child = node.left;
        int second_child = node.right;
        long double first_bound = first_child == -1
            ? std::numeric_limits<long double>::infinity()
            : box_squared_distance(
                state->nodes[static_cast<std::size_t>(first_child)], x, y
            );
        long double second_bound = second_child == -1
            ? std::numeric_limits<long double>::infinity()
            : box_squared_distance(
                state->nodes[static_cast<std::size_t>(second_child)], x, y
            );
        if(second_bound < first_bound){
            std::swap(first_child, second_child);
            std::swap(first_bound, second_bound);
        }
        if(first_bound <= result.squared_distance){
            nearest_neighbor_rec(first_child, x, y, result);
        }
        if(second_bound <= result.squared_distance){
            nearest_neighbor_rec(second_child, x, y, result);
        }
    }

public:
    KdTree2D(): state(std::make_unique<State>()){}

    explicit KdTree2D(const std::vector<Point>& points): KdTree2D(){
        build(points);
    }

    KdTree2D(const KdTree2D&) = delete;
    KdTree2D& operator=(const KdTree2D&) = delete;
    KdTree2D(KdTree2D&&) = default;
    KdTree2D& operator=(KdTree2D&&) = default;

    int size() const{ return point_count; }
    bool empty() const{ return point_count == 0; }

    void clear(){
        point_count = 0;
        node_count = 0;
        root_node = -1;
    }

    void build(const std::vector<Point>& points){
        if(points.size() > static_cast<std::size_t>(MAX_POINTS))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (KdTree2D::build)."
            );
        }
        point_count = static_cast<int>(points.size());
        node_count = 0;
        root_node = -1;
        for(int i = 0; i < point_count; i++){
            state->work[static_cast<std::size_t>(i)] = points[static_cast<std::size_t>(i)];
        }
        root_node = build_rec(0, point_count, 0);
    }

    std::vector<int> range_search(
        const T& min_x,
        const T& max_x,
        const T& min_y,
        const T& max_y
    ) const{
        if(less_value(max_x, min_x) || less_value(max_y, min_y))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (KdTree2D::range_search)."
            );
        }
        std::vector<int> result;
        range_search_rec(root_node, min_x, max_x, min_y, max_y, result);
        return result;
    }

    KdTreeNearestResult2D nearest_neighbor(const T& x, const T& y) const{
        const long double query_x = static_cast<long double>(x);
        const long double query_y = static_cast<long double>(y);
        if(!std::isfinite(query_x) || !std::isfinite(query_y))[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: non-finite query (KdTree2D::nearest_neighbor)."
            );
        }
        KdTreeNearestResult2D result;
        nearest_neighbor_rec(root_node, x, y, result);
        return result;
    }
};
