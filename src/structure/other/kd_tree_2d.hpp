#pragma once

#include <algorithm>
#include <array>
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
};
