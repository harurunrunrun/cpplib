#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_FULLY_DYNAMIC_CONVEX_HULL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_FULLY_DYNAMIC_CONVEX_HULL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "../../scalar/point_point/dot.hpp"

class FullyDynamicConvexHull{
    struct ChainNode;
    using Chain = std::shared_ptr<const ChainNode>;
    struct ChainNode{
        Point point;
        std::uint64_t priority;
        std::size_t size;
        Chain left;
        Chain right;
    };
    struct HullNode{
        long double x;
        long double minimum_y;
        long double maximum_y;
        std::uint64_t priority;
        std::unique_ptr<HullNode> left;
        std::unique_ptr<HullNode> right;
        Chain lower;
        Chain upper;
    };

public:
    using Id = std::size_t;

    bool insert(Id id, const Point& point){
        plus_geometry_detail::validate_finite(point);
        if(points_.contains(id)) return false;
        points_.emplace(id, point);
        auto [x_iterator, new_x] = coordinates_.try_emplace(point.x);
        auto& y_counts = x_iterator->second;
        const long double old_minimum_y = new_x
            ? point.y : y_counts.begin()->first;
        const long double old_maximum_y = new_x
            ? point.y : y_counts.rbegin()->first;
        ++y_counts[point.y];
        if(new_x){
            insert_x(root_, std::make_unique<HullNode>(HullNode{
                point.x, point.y, point.y, next_priority(),
                nullptr, nullptr, nullptr, nullptr,
            }));
        }else if(old_minimum_y != y_counts.begin()->first
                 || old_maximum_y != y_counts.rbegin()->first){
            update_x(root_, point.x, y_counts.begin()->first,
                     y_counts.rbegin()->first);
        }
        return true;
    }

    bool erase(Id id){
        const auto point_iterator = points_.find(id);
        if(point_iterator == points_.end()) return false;
        const Point point = point_iterator->second;
        points_.erase(point_iterator);
        auto x_iterator = coordinates_.find(point.x);
        auto& y_counts = x_iterator->second;
        auto y_iterator = y_counts.find(point.y);
        const long double old_minimum_y = y_counts.begin()->first;
        const long double old_maximum_y = y_counts.rbegin()->first;
        if(--y_iterator->second == 0) y_counts.erase(y_iterator);
        if(y_counts.empty()){
            erase_x(root_, point.x);
            coordinates_.erase(x_iterator);
        }else if(old_minimum_y != y_counts.begin()->first
                 || old_maximum_y != y_counts.rbegin()->first){
            update_x(root_, point.x, y_counts.begin()->first,
                     y_counts.rbegin()->first);
        }
        return true;
    }

    bool contains_id(Id id) const{ return points_.contains(id); }
    std::size_t size() const{ return points_.size(); }
    bool empty() const{ return points_.empty(); }

    std::vector<Point> vertices() const{
        if(root_ == nullptr) return {};
        std::vector<Point> lower;
        std::vector<Point> upper;
        lower.reserve(chain_size(root_->lower));
        upper.reserve(chain_size(root_->upper));
        append_chain(root_->lower, lower);
        append_chain(root_->upper, upper);
        std::vector<Point> result;
        result.reserve(lower.size() + upper.size());
        for(const Point& point: lower) append_distinct(result, point);
        for(std::size_t index = upper.size(); index-- > 0;){
            append_distinct(result, upper[index]);
        }
        if(result.size() >= 2 && exactly_equal(result.front(), result.back())){
            result.pop_back();
        }
        if(result.size() == 3
            && plus_geometry_detail::orientation(result[0], result[1], result[2])
                == 0){
            result.erase(result.begin() + 1);
        }
        return result;
    }

    int contains(const Point& point) const{
        plus_geometry_detail::validate_finite(point);
        if(root_ == nullptr) return 0;
        const Chain& lower = root_->lower;
        const Chain& upper = root_->upper;
        const long double minimum_x = chain_front(lower).x;
        const long double maximum_x = chain_back(lower).x;
        const long double x_scale =
            std::max({1.0L, std::abs(minimum_x), std::abs(maximum_x)});
        if(plus_geometry_detail::sign(point.x - minimum_x, x_scale) < 0
            || plus_geometry_detail::sign(point.x - maximum_x, x_scale) > 0){
            return 0;
        }
        const long double lower_y = chain_y_at(lower, point.x);
        const long double upper_y = chain_y_at(upper, point.x);
        const long double y_scale = std::max({
            1.0L, std::abs(point.y), std::abs(lower_y), std::abs(upper_y)
        });
        const int below = plus_geometry_detail::sign(point.y - lower_y, y_scale);
        const int above = plus_geometry_detail::sign(point.y - upper_y, y_scale);
        if(below < 0 || above > 0) return 0;
        return below == 0 || above == 0 ? 1 : 2;
    }

    std::optional<Point> support(const Point& direction) const{
        plus_geometry_detail::validate_finite(direction);
        if(root_ == nullptr) return std::nullopt;
        if(direction.x == 0.0L && direction.y == 0.0L){
            return chain_front(root_->lower);
        }
        const Chain& chain = direction.y >= 0.0L ? root_->upper : root_->lower;
        std::size_t low = 0;
        std::size_t high = chain_size(chain) - 1;
        while(low < high){
            const std::size_t middle = low + (high - low) / 2;
            const long double first = dot(chain_at(chain, middle), direction);
            const long double second = dot(chain_at(chain, middle + 1), direction);
            if(first < second) low = middle + 1;
            else high = middle;
        }
        return chain_at(chain, low);
    }

private:
    static bool exactly_equal(const Point& first, const Point& second){
        return first.x == second.x && first.y == second.y;
    }
    static void append_distinct(std::vector<Point>& points, const Point& point){
        if(points.empty() || !exactly_equal(points.back(), point)){
            points.push_back(point);
        }
    }
    static std::size_t chain_size(const Chain& chain){
        return chain == nullptr ? 0 : chain->size;
    }
    static Chain chain_node(const Point& point, std::uint64_t priority,
                            Chain left, Chain right){
        return std::make_shared<const ChainNode>(ChainNode{
            point, priority, 1 + chain_size(left) + chain_size(right),
            std::move(left), std::move(right),
        });
    }
    static Chain concatenate(Chain left, Chain right){
        if(left == nullptr) return right;
        if(right == nullptr) return left;
        if(left->priority > right->priority){
            return chain_node(left->point, left->priority, left->left,
                              concatenate(left->right, std::move(right)));
        }
        return chain_node(right->point, right->priority,
                          concatenate(std::move(left), right->left), right->right);
    }
    static Chain prefix(const Chain& chain, std::size_t count){
        if(count == 0 || chain == nullptr) return nullptr;
        if(count >= chain_size(chain)) return chain;
        const std::size_t left_size = chain_size(chain->left);
        if(count <= left_size) return prefix(chain->left, count);
        return chain_node(chain->point, chain->priority, chain->left,
                          prefix(chain->right, count - left_size - 1));
    }
    static Chain suffix(const Chain& chain, std::size_t first){
        if(chain == nullptr || first >= chain_size(chain)) return nullptr;
        if(first == 0) return chain;
        const std::size_t left_size = chain_size(chain->left);
        if(first <= left_size){
            return chain_node(chain->point, chain->priority,
                              suffix(chain->left, first), chain->right);
        }
        return suffix(chain->right, first - left_size - 1);
    }
    static const Point& chain_at(const Chain& chain, std::size_t index){
        const std::size_t left_size = chain_size(chain->left);
        if(index < left_size) return chain_at(chain->left, index);
        if(index == left_size) return chain->point;
        return chain_at(chain->right, index - left_size - 1);
    }
    static const Point& chain_front(const Chain& chain){
        const ChainNode* node = chain.get();
        while(node->left != nullptr) node = node->left.get();
        return node->point;
    }
    static const Point& chain_back(const Chain& chain){
        const ChainNode* node = chain.get();
        while(node->right != nullptr) node = node->right.get();
        return node->point;
    }
    static void append_chain(const Chain& chain, std::vector<Point>& result){
        if(chain == nullptr) return;
        append_chain(chain->left, result);
        result.push_back(chain->point);
        append_chain(chain->right, result);
    }
    static long double directed_slope(const Point& first, const Point& second,
                                      int direction){
        return static_cast<long double>(direction)
            * (second.y - first.y) / (second.x - first.x);
    }
    static std::pair<std::size_t, long double> minimum_right_slope(
        const Point& point, const Chain& right, int direction
    ){
        std::size_t low = 0;
        std::size_t high = chain_size(right) - 1;
        while(low < high){
            const std::size_t middle = low + (high - low) / 2;
            const long double first = directed_slope(
                point, chain_at(right, middle), direction);
            const long double second = directed_slope(
                point, chain_at(right, middle + 1), direction);
            if(first >= second) low = middle + 1;
            else high = middle;
        }
        return {low, directed_slope(point, chain_at(right, low), direction)};
    }
    static Chain merge_chain(const Chain& left, const Chain& right,
                             int direction){
        if(left == nullptr) return right;
        if(right == nullptr) return left;
        std::size_t low = 0;
        std::size_t high = chain_size(left) - 1;
        while(low < high){
            const std::size_t middle = low + (high - low) / 2;
            const long double first = minimum_right_slope(
                chain_at(left, middle), right, direction).second;
            const long double second = minimum_right_slope(
                chain_at(left, middle + 1), right, direction).second;
            if(first < second) low = middle + 1;
            else high = middle;
        }
        const std::size_t left_index = low;
        const std::size_t right_index = minimum_right_slope(
            chain_at(left, left_index), right, direction).first;
        return concatenate(prefix(left, left_index + 1),
                           suffix(right, right_index));
    }
    static long double chain_y_at(const Chain& chain, long double x){
        const ChainNode* node = chain.get();
        const Point* previous = nullptr;
        const Point* next = nullptr;
        while(node != nullptr){
            if(node->point.x <= x){
                previous = &node->point;
                node = node->right.get();
            }else{
                next = &node->point;
                node = node->left.get();
            }
        }
        if(previous == nullptr) return next->y;
        if(next == nullptr || previous->x == x) return previous->y;
        return previous->y + (next->y - previous->y)
            * (x - previous->x) / (next->x - previous->x);
    }
    static void pull(HullNode& node){
        const Chain lower_point = chain_node(
            {node.x, node.minimum_y}, node.priority, nullptr, nullptr);
        const Chain upper_point = chain_node(
            {node.x, node.maximum_y}, node.priority, nullptr, nullptr);
        node.lower = lower_point;
        node.upper = upper_point;
        if(node.left != nullptr){
            node.lower = merge_chain(node.left->lower, node.lower, 1);
            node.upper = merge_chain(node.left->upper, node.upper, -1);
        }
        if(node.right != nullptr){
            node.lower = merge_chain(node.lower, node.right->lower, 1);
            node.upper = merge_chain(node.upper, node.right->upper, -1);
        }
    }
    static void split_x(std::unique_ptr<HullNode> root, long double x,
                        std::unique_ptr<HullNode>& left,
                        std::unique_ptr<HullNode>& right){
        if(root == nullptr){
            left = nullptr;
            right = nullptr;
            return;
        }
        if(root->x < x){
            split_x(std::move(root->right), x, root->right, right);
            pull(*root);
            left = std::move(root);
        }else{
            split_x(std::move(root->left), x, left, root->left);
            pull(*root);
            right = std::move(root);
        }
    }
    static void insert_x(std::unique_ptr<HullNode>& root,
                         std::unique_ptr<HullNode> node){
        if(root == nullptr){
            pull(*node);
            root = std::move(node);
            return;
        }
        if(node->priority > root->priority){
            split_x(std::move(root), node->x, node->left, node->right);
            pull(*node);
            root = std::move(node);
        }else if(node->x < root->x){
            insert_x(root->left, std::move(node));
            pull(*root);
        }else{
            insert_x(root->right, std::move(node));
            pull(*root);
        }
    }
    static std::unique_ptr<HullNode> merge_x(
        std::unique_ptr<HullNode> left, std::unique_ptr<HullNode> right
    ){
        if(left == nullptr) return right;
        if(right == nullptr) return left;
        if(left->priority > right->priority){
            left->right = merge_x(std::move(left->right), std::move(right));
            pull(*left);
            return left;
        }
        right->left = merge_x(std::move(left), std::move(right->left));
        pull(*right);
        return right;
    }
    static void erase_x(std::unique_ptr<HullNode>& root, long double x){
        if(root->x == x){
            root = merge_x(std::move(root->left), std::move(root->right));
        }else if(x < root->x){
            erase_x(root->left, x);
            pull(*root);
        }else{
            erase_x(root->right, x);
            pull(*root);
        }
    }
    static void update_x(std::unique_ptr<HullNode>& root, long double x,
                         long double minimum_y, long double maximum_y){
        if(root->x == x){
            root->minimum_y = minimum_y;
            root->maximum_y = maximum_y;
        }else if(x < root->x){
            update_x(root->left, x, minimum_y, maximum_y);
        }else{
            update_x(root->right, x, minimum_y, maximum_y);
        }
        pull(*root);
    }
    std::uint64_t next_priority(){
        return random_engine_();
    }

    std::map<Id, Point> points_;
    std::map<long double, std::map<long double, std::size_t>> coordinates_;
    std::unique_ptr<HullNode> root_;
    std::mt19937_64 random_engine_{std::random_device{}()};
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_FULLY_DYNAMIC_CONVEX_HULL_HPP_INCLUDED
