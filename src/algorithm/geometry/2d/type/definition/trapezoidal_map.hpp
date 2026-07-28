#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_TRAPEZOIDAL_MAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_TRAPEZOIDAL_MAP_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"

struct TrapezoidalMapLocation{
    static constexpr std::size_t no_segment = static_cast<std::size_t>(-1);
    std::size_t slab = 0;
    std::size_t below = no_segment;
    std::size_t above = no_segment;
    std::size_t on_segment = no_segment;
};

class TrapezoidalMap{
    struct Node;
    using Root = std::shared_ptr<const Node>;

    struct Node{
        std::size_t segment;
        std::size_t height;
        Root left;
        Root right;
    };

public:
    TrapezoidalMap() = default;

    explicit TrapezoidalMap(std::vector<Segment> segments)
        : segments_(std::move(segments)){
        for(std::size_t index = 0; index < segments_.size(); ++index){
            const Segment& segment = segments_[index];
            plus_geometry_detail::validate_finite(segment.a);
            plus_geometry_detail::validate_finite(segment.b);
            if(plus_geometry_detail::close(segment.a, segment.b)){
                throw std::invalid_argument(
                    "a trapezoidal-map segment requires distinct endpoints"
                );
            }
            x_events_.push_back(segment.a.x);
            x_events_.push_back(segment.b.x);
            endpoints_[segment.a.x].push_back({segment.a.y, index});
            endpoints_[segment.b.x].push_back({segment.b.y, index});
            if(plus_geometry_detail::sign(segment.a.x - segment.b.x) == 0){
                vertical_[segment.a.x].push_back({
                    std::min(segment.a.y, segment.b.y),
                    std::max(segment.a.y, segment.b.y),
                    index,
                });
            }
        }
        for(auto& [x, records]: endpoints_){
            static_cast<void>(x);
            std::sort(records.begin(), records.end());
        }
        for(auto& [x, records]: vertical_){
            static_cast<void>(x);
            std::sort(records.begin(), records.end());
        }
        std::sort(x_events_.begin(), x_events_.end());
        x_events_.erase(
            std::unique(x_events_.begin(), x_events_.end()),
            x_events_.end()
        );
        if(x_events_.empty()) return;
        build_persistent_slabs();
    }

    TrapezoidalMapLocation locate(const Point& point) const{
        plus_geometry_detail::validate_finite(point);
        TrapezoidalMapLocation result;
        if(const std::optional<std::size_t> event = event_segment(point)){
            result.on_segment = *event;
            return result;
        }
        if(roots_.empty()) return result;
        result.slab = static_cast<std::size_t>(
            std::upper_bound(x_events_.begin(), x_events_.end(), point.x)
                - x_events_.begin()
        );
        Root current = roots_[result.slab];
        while(current != nullptr){
            const long double y = y_at(segments_[current->segment], point.x);
            if(plus_geometry_detail::sign(y - point.y) < 0){
                result.below = current->segment;
                current = current->right;
            }else{
                result.above = current->segment;
                current = current->left;
            }
        }
        for(std::size_t candidate: {result.below, result.above}){
            if(candidate == TrapezoidalMapLocation::no_segment) continue;
            const Segment& segment = segments_[candidate];
            if(plus_geometry_detail::orientation(segment.a, segment.b, point) == 0
                && plus_geometry_detail::sign(
                    dot(point - segment.a, point - segment.b),
                    plus_geometry_detail::squared_distance(segment.a, segment.b)
                ) <= 0){
                result.on_segment = candidate;
            }
        }
        return result;
    }

    std::size_t segment_count() const{
        return segments_.size();
    }

    std::size_t slab_count() const{
        return roots_.size();
    }

private:
    struct VerticalRecord{
        long double minimum_y;
        long double maximum_y;
        std::size_t index;
        bool operator<(const VerticalRecord& other) const{
            if(minimum_y != other.minimum_y) return minimum_y < other.minimum_y;
            return index < other.index;
        }
    };

    template<class Records>
    static auto close_x_record(const Records& records, long double x){
        auto iterator = records.lower_bound(x);
        if(iterator != records.begin()){
            const auto previous = std::prev(iterator);
            if(plus_geometry_detail::sign(previous->first - x) == 0){
                return previous;
            }
        }
        return iterator;
    }

    std::optional<std::size_t> event_segment(const Point& point) const{
        auto endpoints = close_x_record(endpoints_, point.x);
        if(endpoints != endpoints_.end()
            && plus_geometry_detail::sign(endpoints->first - point.x) == 0){
            auto iterator = std::lower_bound(
                endpoints->second.begin(), endpoints->second.end(),
                std::pair{point.y, std::size_t{0}}
            );
            for(int offset = -1; offset <= 1; ++offset){
                if(offset < 0 && iterator == endpoints->second.begin()) continue;
                auto candidate = iterator;
                if(offset < 0) --candidate;
                else if(offset > 0 && candidate != endpoints->second.end()) ++candidate;
                if(candidate != endpoints->second.end()
                    && plus_geometry_detail::sign(candidate->first - point.y) == 0){
                    return candidate->second;
                }
            }
        }
        auto vertical = close_x_record(vertical_, point.x);
        if(vertical == vertical_.end()
            || plus_geometry_detail::sign(vertical->first - point.x) != 0){
            return std::nullopt;
        }
        auto iterator = std::upper_bound(
            vertical->second.begin(), vertical->second.end(),
            VerticalRecord{point.y, point.y, 0}
        );
        if(iterator == vertical->second.begin()) return std::nullopt;
        --iterator;
        if(plus_geometry_detail::sign(point.y - iterator->maximum_y) <= 0){
            return iterator->index;
        }
        return std::nullopt;
    }

    static long double y_at(const Segment& segment, long double x){
        return segment.a.y + (segment.b.y - segment.a.y)
            * (x - segment.a.x) / (segment.b.x - segment.a.x);
    }

    static std::size_t height(const Root& root){
        return root == nullptr ? 0 : root->height;
    }

    bool less(std::size_t first, std::size_t second, long double x) const{
        const long double first_y = y_at(segments_[first], x);
        const long double second_y = y_at(segments_[second], x);
        if(first_y != second_y) return first_y < second_y;
        return first < second;
    }

    Root node(std::size_t segment, Root left, Root right) const{
        return std::make_shared<const Node>(Node{
            segment,
            1 + std::max(height(left), height(right)),
            std::move(left),
            std::move(right)
        });
    }

    Root rotate_left(const Root& root) const{
        const Root& right = root->right;
        return node(
            right->segment,
            node(root->segment, root->left, right->left),
            right->right
        );
    }

    Root rotate_right(const Root& root) const{
        const Root& left = root->left;
        return node(
            left->segment,
            left->left,
            node(root->segment, left->right, root->right)
        );
    }

    Root balance(Root root) const{
        if(root == nullptr) return nullptr;
        const int difference = static_cast<int>(height(root->left))
            - static_cast<int>(height(root->right));
        if(difference > 1){
            if(height(root->left->left) < height(root->left->right)){
                root = node(
                    root->segment, rotate_left(root->left), root->right
                );
            }
            return rotate_right(root);
        }
        if(difference < -1){
            if(height(root->right->right) < height(root->right->left)){
                root = node(
                    root->segment, root->left, rotate_right(root->right)
                );
            }
            return rotate_left(root);
        }
        return root;
    }

    Root insert(Root root, std::size_t segment, long double x) const{
        if(root == nullptr) return node(segment, nullptr, nullptr);
        if(less(segment, root->segment, x)){
            return balance(node(
                root->segment, insert(root->left, segment, x), root->right
            ));
        }
        return balance(node(
            root->segment, root->left, insert(root->right, segment, x)
        ));
    }


    std::pair<Root, std::size_t> extract_min(const Root& root) const{
        if(root->left == nullptr) return {root->right, root->segment};
        auto [left, segment] = extract_min(root->left);
        return {
            balance(node(root->segment, std::move(left), root->right)),
            segment
        };
    }

    Root erase(Root root, std::size_t segment, long double x) const{
        if(root == nullptr) return nullptr;
        if(root->segment == segment){
            if(root->left == nullptr) return root->right;
            if(root->right == nullptr) return root->left;
            auto [right, replacement] = extract_min(root->right);
            return balance(node(
                replacement, root->left, std::move(right)
            ));
        }
        if(less(segment, root->segment, x)){
            return balance(node(
                root->segment, erase(root->left, segment, x), root->right
            ));
        }
        return balance(node(
            root->segment, root->left, erase(root->right, segment, x)
        ));
    }

    void build_persistent_slabs(){
        std::vector<std::vector<std::size_t>> starts(x_events_.size());
        std::vector<std::vector<std::size_t>> ends(x_events_.size());
        for(std::size_t segment = 0; segment < segments_.size(); ++segment){
            const long double minimum_x = std::min(
                segments_[segment].a.x, segments_[segment].b.x
            );
            const long double maximum_x = std::max(
                segments_[segment].a.x, segments_[segment].b.x
            );
            if(minimum_x == maximum_x) continue;
            const std::size_t first = static_cast<std::size_t>(
                std::lower_bound(x_events_.begin(), x_events_.end(), minimum_x)
                    - x_events_.begin()
            );
            const std::size_t last = static_cast<std::size_t>(
                std::lower_bound(x_events_.begin(), x_events_.end(), maximum_x)
                    - x_events_.begin()
            );
            starts[first].push_back(segment);
            ends[last].push_back(segment);
        }
        roots_.assign(x_events_.size() + 1, nullptr);
        Root root;
        for(std::size_t event = 0; event < x_events_.size(); ++event){
            const long double left_sample = event == 0
                ? x_events_[event] - 1.0L
                : (x_events_[event - 1] + x_events_[event]) / 2.0L;
            for(std::size_t segment: ends[event]){
                root = erase(root, segment, left_sample);
            }
            const long double right_sample = event + 1 == x_events_.size()
                ? x_events_[event] + 1.0L
                : (x_events_[event] + x_events_[event + 1]) / 2.0L;
            for(std::size_t segment: starts[event]){
                root = insert(root, segment, right_sample);
            }
            roots_[event + 1] = root;
        }
    }

    std::vector<Segment> segments_;
    std::vector<long double> x_events_;
    std::vector<Root> roots_;
    std::map<long double, std::vector<std::pair<long double, std::size_t>>>
        endpoints_;
    std::map<long double, std::vector<VerticalRecord>> vertical_;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_TRAPEZOIDAL_MAP_HPP_INCLUDED
