#ifndef CPPLIB_SRC_STRUCTURE_OTHER_POINT_ADD_RECTANGLE_AFFINE_RECTANGLE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_POINT_ADD_RECTANGLE_AFFINE_RECTANGLE_SUM_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct PointAddRectangleAffineRectangleSum{
    using coordinate_type = std::int64_t;

    struct Point{
        coordinate_type x;
        coordinate_type y;
        T value;
    };

private:
    struct Node{
        coordinate_type x;
        coordinate_type y;
        coordinate_type minimum_x;
        coordinate_type maximum_x;
        coordinate_type minimum_y;
        coordinate_type maximum_y;
        T value;
        T sum;
        T lazy_multiplier{1};
        T lazy_addend{};
        int left = -1;
        int right = -1;
        int count = 1;
        int axis;

        Node(const Point& point, int split_axis):
            x(point.x),
            y(point.y),
            minimum_x(point.x),
            maximum_x(point.x),
            minimum_y(point.y),
            maximum_y(point.y),
            value(point.value),
            sum(point.value),
            axis(split_axis){}
    };

    int root = -1;
    std::vector<Node> nodes;
    std::vector<Point> insertion_buffer;

    static T scale(const T& value, int multiplier){
        return value * multiplier;
    }

    static bool point_less(
        coordinate_type first_x,
        coordinate_type first_y,
        coordinate_type second_x,
        coordinate_type second_y,
        int axis
    ){
        if(axis == 0){
            if(first_x != second_x) return first_x < second_x;
            return first_y < second_y;
        }
        if(first_y != second_y) return first_y < second_y;
        return first_x < second_x;
    }

    static bool point_in_rectangle(
        coordinate_type x,
        coordinate_type y,
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper
    ){
        return x_left <= x && x < x_right &&
               y_lower <= y && y < y_upper;
    }

    bool disjoint(
        const Node& node,
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper
    ) const{
        return node.maximum_x < x_left || x_right <= node.minimum_x ||
               node.maximum_y < y_lower || y_upper <= node.minimum_y;
    }

    bool fully_covered(
        const Node& node,
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper
    ) const{
        return x_left <= node.minimum_x && node.maximum_x < x_right &&
               y_lower <= node.minimum_y && node.maximum_y < y_upper;
    }

    void check_rectangle(
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper
    ) const{
        if(x_right < x_left || y_upper < y_lower)[[unlikely]]{
            throw std::invalid_argument(
                "PointAddRectangleAffineRectangleSum: invalid rectangle"
            );
        }
    }

    int node_count(int index) const{
        return index == -1 ? 0 : nodes[index].count;
    }

    T node_sum(int index) const{
        return index == -1 ? T{} : nodes[index].sum;
    }

    void apply_node(int index, const T& multiplier, const T& addend){
        if(index == -1) return;
        Node& node = nodes[index];
        node.value = multiplier * node.value + addend;
        node.sum = multiplier * node.sum + scale(addend, node.count);
        node.lazy_multiplier = multiplier * node.lazy_multiplier;
        node.lazy_addend = multiplier * node.lazy_addend + addend;
    }

    void push(int index){
        if(index == -1) return;
        const T multiplier = nodes[index].lazy_multiplier;
        const T addend = nodes[index].lazy_addend;
        apply_node(nodes[index].left, multiplier, addend);
        apply_node(nodes[index].right, multiplier, addend);
        nodes[index].lazy_multiplier = T{1};
        nodes[index].lazy_addend = T{};
    }

    void pull(int index){
        Node& node = nodes[index];
        node.count = 1 + node_count(node.left) + node_count(node.right);
        node.sum = node_sum(node.left) + node.value + node_sum(node.right);
        node.minimum_x = node.maximum_x = node.x;
        node.minimum_y = node.maximum_y = node.y;
        if(node.left != -1){
            const Node& child = nodes[node.left];
            node.minimum_x = std::min(node.minimum_x, child.minimum_x);
            node.maximum_x = std::max(node.maximum_x, child.maximum_x);
            node.minimum_y = std::min(node.minimum_y, child.minimum_y);
            node.maximum_y = std::max(node.maximum_y, child.maximum_y);
        }
        if(node.right != -1){
            const Node& child = nodes[node.right];
            node.minimum_x = std::min(node.minimum_x, child.minimum_x);
            node.maximum_x = std::max(node.maximum_x, child.maximum_x);
            node.minimum_y = std::min(node.minimum_y, child.minimum_y);
            node.maximum_y = std::max(node.maximum_y, child.maximum_y);
        }
    }

    int build(
        std::vector<Point>& points,
        std::size_t begin,
        std::size_t end,
        int depth
    ){
        if(begin == end) return -1;
        const std::size_t middle = begin + (end - begin) / 2;
        const int axis = depth & 1;
        std::nth_element(
            points.begin() + static_cast<std::ptrdiff_t>(begin),
            points.begin() + static_cast<std::ptrdiff_t>(middle),
            points.begin() + static_cast<std::ptrdiff_t>(end),
            [axis](const Point& first, const Point& second){
                return point_less(
                    first.x,
                    first.y,
                    second.x,
                    second.y,
                    axis
                );
            }
        );
        const int index = static_cast<int>(nodes.size());
        nodes.emplace_back(points[middle], axis);
        const int left = build(points, begin, middle, depth + 1);
        const int right = build(points, middle + 1, end, depth + 1);
        nodes[index].left = left;
        nodes[index].right = right;
        pull(index);
        return index;
    }

    void collect_points(int index, std::vector<Point>& points){
        if(index == -1) return;
        push(index);
        collect_points(nodes[index].left, points);
        points.push_back({nodes[index].x, nodes[index].y, nodes[index].value});
        collect_points(nodes[index].right, points);
    }

    void rebuild_all(){
        std::vector<Point> points;
        points.reserve(size());
        collect_points(root, points);
        points.insert(
            points.end(),
            insertion_buffer.begin(),
            insertion_buffer.end()
        );
        insertion_buffer.clear();
        nodes.clear();
        nodes.reserve(points.size());
        root = build(points, 0, points.size(), 0);
    }

    bool add_existing_point(
        int index,
        coordinate_type x,
        coordinate_type y,
        const T& delta
    ){
        if(index == -1) return false;
        push(index);
        bool found = false;
        if(nodes[index].x == x && nodes[index].y == y){
            nodes[index].value = nodes[index].value + delta;
            found = true;
        }else if(point_less(
            x,
            y,
            nodes[index].x,
            nodes[index].y,
            nodes[index].axis
        )){
            found = add_existing_point(nodes[index].left, x, y, delta);
        }else{
            found = add_existing_point(nodes[index].right, x, y, delta);
        }
        if(found) pull(index);
        return found;
    }

    void rectangle_affine_tree(
        int index,
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper,
        const T& multiplier,
        const T& addend
    ){
        if(index == -1 || disjoint(
            nodes[index], x_left, x_right, y_lower, y_upper
        )) return;
        if(fully_covered(
            nodes[index], x_left, x_right, y_lower, y_upper
        )){
            apply_node(index, multiplier, addend);
            return;
        }
        push(index);
        if(point_in_rectangle(
            nodes[index].x,
            nodes[index].y,
            x_left,
            x_right,
            y_lower,
            y_upper
        )){
            nodes[index].value = multiplier * nodes[index].value + addend;
        }
        rectangle_affine_tree(
            nodes[index].left,
            x_left,
            x_right,
            y_lower,
            y_upper,
            multiplier,
            addend
        );
        rectangle_affine_tree(
            nodes[index].right,
            x_left,
            x_right,
            y_lower,
            y_upper,
            multiplier,
            addend
        );
        pull(index);
    }

    T rectangle_sum_tree(
        int index,
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper
    ){
        if(index == -1 || disjoint(
            nodes[index], x_left, x_right, y_lower, y_upper
        )) return T{};
        if(fully_covered(
            nodes[index], x_left, x_right, y_lower, y_upper
        )) return nodes[index].sum;
        push(index);
        T result{};
        if(point_in_rectangle(
            nodes[index].x,
            nodes[index].y,
            x_left,
            x_right,
            y_lower,
            y_upper
        )) result = result + nodes[index].value;
        result = result + rectangle_sum_tree(
            nodes[index].left,
            x_left,
            x_right,
            y_lower,
            y_upper
        );
        result = result + rectangle_sum_tree(
            nodes[index].right,
            x_left,
            x_right,
            y_lower,
            y_upper
        );
        return result;
    }

    void rebuild_if_needed(){
        const std::size_t total_size = size();
        const std::size_t buffered = insertion_buffer.size();
        if(buffered != 0 && buffered > total_size / buffered){
            rebuild_all();
        }
    }

public:
    PointAddRectangleAffineRectangleSum() = default;

    explicit PointAddRectangleAffineRectangleSum(
        const std::vector<Point>& initial_points
    ){
        std::vector<Point> points = initial_points;
        std::sort(points.begin(), points.end(), [](const Point& first, const Point& second){
            if(first.x != second.x) return first.x < second.x;
            return first.y < second.y;
        });
        for(const Point& point: points){
            if(!insertion_buffer.empty() &&
               insertion_buffer.back().x == point.x &&
               insertion_buffer.back().y == point.y){
                insertion_buffer.back().value =
                    insertion_buffer.back().value + point.value;
            }else{
                insertion_buffer.push_back(point);
            }
        }
        rebuild_all();
    }

    std::size_t size() const{
        return (root == -1 ? 0U : static_cast<std::size_t>(nodes[root].count))
            + insertion_buffer.size();
    }

    void clear(){
        root = -1;
        nodes.clear();
        insertion_buffer.clear();
    }

    bool point_add(coordinate_type x, coordinate_type y, const T& delta){
        if(add_existing_point(root, x, y, delta)) return false;
        for(Point& point: insertion_buffer){
            if(point.x == x && point.y == y){
                point.value = point.value + delta;
                return false;
            }
        }
        insertion_buffer.push_back({x, y, delta});
        rebuild_if_needed();
        return true;
    }

    void rectangle_affine(
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper,
        const T& multiplier,
        const T& addend
    ){
        check_rectangle(x_left, x_right, y_lower, y_upper);
        if(x_left == x_right || y_lower == y_upper) return;
        rectangle_affine_tree(
            root,
            x_left,
            x_right,
            y_lower,
            y_upper,
            multiplier,
            addend
        );
        for(Point& point: insertion_buffer){
            if(point_in_rectangle(
                point.x,
                point.y,
                x_left,
                x_right,
                y_lower,
                y_upper
            )) point.value = multiplier * point.value + addend;
        }
    }

    T rectangle_sum(
        coordinate_type x_left,
        coordinate_type x_right,
        coordinate_type y_lower,
        coordinate_type y_upper
    ){
        check_rectangle(x_left, x_right, y_lower, y_upper);
        if(x_left == x_right || y_lower == y_upper) return T{};
        T result = rectangle_sum_tree(
            root,
            x_left,
            x_right,
            y_lower,
            y_upper
        );
        for(const Point& point: insertion_buffer){
            if(point_in_rectangle(
                point.x,
                point.y,
                x_left,
                x_right,
                y_lower,
                y_upper
            )) result = result + point.value;
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_POINT_ADD_RECTANGLE_AFFINE_RECTANGLE_SUM_HPP_INCLUDED
