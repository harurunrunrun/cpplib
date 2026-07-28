#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_PLANAR_POINT_LOCATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_PLANAR_POINT_LOCATION_HPP_INCLUDED

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

struct PlanarPointLocationResult{
    static constexpr std::size_t outside =
        std::numeric_limits<std::size_t>::max();
    std::size_t face = outside;
    bool on_boundary = false;
};

class PlanarPointLocation{
    struct EdgeRecord{
        Point first;
        Point second;
        std::size_t face;
    };

    struct Node;
    using Root = std::shared_ptr<const Node>;
    struct Node{
        std::size_t edge;
        int height;
        Root left;
        Root right;
    };

    struct VerticalRecord{
        long double lower;
        long double upper;
        std::size_t face;
        long double prefix_upper;
        std::size_t prefix_face;
        bool operator<(const VerticalRecord& other) const{
            if(lower != other.lower) return lower < other.lower;
            if(upper != other.upper) return upper < other.upper;
            return face < other.face;
        }
    };

public:
    PlanarPointLocation() = default;

    explicit PlanarPointLocation(std::vector<std::vector<Point>> faces)
        : faces_(std::move(faces)){
        for(std::size_t face = 0; face < faces_.size(); ++face){
            faces_[face] = plus_geometry_detail::normalized_polygon(
                std::move(faces_[face])
            );
            if(faces_[face].size() < 3){
                throw std::invalid_argument(
                    "a point-location face requires at least three vertices"
                );
            }
            for(const Point& point: faces_[face]){
                plus_geometry_detail::validate_finite(point);
            }
            if(plus_geometry_detail::sign(
                plus_geometry_detail::signed_area_twice(faces_[face])
            ) == 0){
                throw std::invalid_argument(
                    "a point-location face must have positive area"
                );
            }
            for(const Point& point: faces_[face]){
                events_.push_back(point.x);
                endpoints_[point.x].push_back({point.y, face});
            }
            for(std::size_t index = 0;
                index < faces_[face].size();
                ++index){
                const Point first = faces_[face][index];
                const Point second =
                    faces_[face][(index + 1) % faces_[face].size()];
                const std::size_t edge = edges_.size();
                edges_.push_back({first, second, face});
                if(first.x == second.x){
                    vertical_[first.x].push_back({
                        std::min(first.y, second.y),
                        std::max(first.y, second.y),
                        face,
                        -std::numeric_limits<long double>::infinity(),
                        face,
                    });
                }
                edge_indices_.push_back(edge);
            }
        }
        for(auto& [x, records]: endpoints_){
            static_cast<void>(x);
            std::sort(records.begin(), records.end());
        }
        for(auto& [x, records]: vertical_){
            static_cast<void>(x);
            std::sort(records.begin(), records.end());
            long double maximum_upper =
                -std::numeric_limits<long double>::infinity();
            std::size_t maximum_face = 0;
            for(VerticalRecord& record: records){
                if(record.upper > maximum_upper){
                    maximum_upper = record.upper;
                    maximum_face = record.face;
                }
                record.prefix_upper = maximum_upper;
                record.prefix_face = maximum_face;
            }
        }
        std::sort(events_.begin(), events_.end());
        events_.erase(
            std::unique(events_.begin(), events_.end()), events_.end()
        );
        build_versions();
    }

    PlanarPointLocationResult locate(const Point& point) const{
        plus_geometry_detail::validate_finite(point);
        if(const auto boundary = event_boundary(point)){
            return {*boundary, true};
        }
        if(roots_.empty()) return {};
        const std::size_t version = static_cast<std::size_t>(
            std::upper_bound(events_.begin(), events_.end(), point.x)
                - events_.begin()
        );
        Root current = roots_[version];
        std::size_t below = edge_indices_.size();
        std::size_t above = edge_indices_.size();
        while(current != nullptr){
            const long double y = ordinate(edges_[current->edge], point.x);
            if(y < point.y){
                below = current->edge;
                current = current->right;
            }else{
                above = current->edge;
                current = current->left;
            }
        }
        for(std::size_t edge: {below, above}){
            if(edge == edge_indices_.size()) continue;
            if(on_segment(edges_[edge], point)){
                return {edges_[edge].face, true};
            }
        }
        if(below != edge_indices_.size()
            && edges_[below].second.x > edges_[below].first.x){
            return {edges_[below].face, false};
        }
        if(above != edge_indices_.size()
            && edges_[above].second.x < edges_[above].first.x){
            return {edges_[above].face, false};
        }
        return {};
    }

    std::size_t face_count() const{ return faces_.size(); }
    std::size_t edge_count() const{ return edges_.size(); }

private:

    static long double ordinate(
        const EdgeRecord& edge,
        long double x
    ){
        return edge.first.y + (edge.second.y - edge.first.y)
            * (x - edge.first.x) / (edge.second.x - edge.first.x);
    }

    static bool on_segment(
        const EdgeRecord& edge,
        const Point& point
    ){
        return plus_geometry_detail::orientation(
            edge.first, edge.second, point
        ) == 0 && plus_geometry_detail::sign(
            dot(point - edge.first, point - edge.second),
            plus_geometry_detail::squared_distance(
                edge.first, edge.second
            )
        ) <= 0;
    }

    bool less(
        std::size_t first,
        std::size_t second,
        long double x
    ) const{
        const long double first_y = ordinate(edges_[first], x);
        const long double second_y = ordinate(edges_[second], x);
        if(first_y != second_y) return first_y < second_y;
        return first < second;
    }

    static int height(const Root& root){
        return root == nullptr ? 0 : root->height;
    }

    Root node(std::size_t edge, Root left, Root right) const{
        return std::make_shared<const Node>(Node{
            edge,
            1 + std::max(height(left), height(right)),
            std::move(left),
            std::move(right),
        });
    }

    Root rotate_left(const Root& root) const{
        const Root& right = root->right;
        const Root new_left = node(
            root->edge, root->left, right->left
        );
        return node(right->edge, new_left, right->right);
    }

    Root rotate_right(const Root& root) const{
        const Root& left = root->left;
        const Root new_right = node(
            root->edge, left->right, root->right
        );
        return node(left->edge, left->left, new_right);
    }

    Root balance(Root root) const{
        const int factor = height(root->left) - height(root->right);
        if(factor > 1){
            if(height(root->left->left) < height(root->left->right)){
                root = node(
                    root->edge, rotate_left(root->left), root->right
                );
            }
            return rotate_right(root);
        }
        if(factor < -1){
            if(height(root->right->right) < height(root->right->left)){
                root = node(
                    root->edge, root->left, rotate_right(root->right)
                );
            }
            return rotate_left(root);
        }
        return root;
    }

    Root insert(
        Root root,
        std::size_t edge,
        long double x
    ) const{
        if(root == nullptr) return node(edge, nullptr, nullptr);
        if(less(edge, root->edge, x)){
            return balance(node(
                root->edge,
                insert(root->left, edge, x),
                root->right
            ));
        }
        return balance(node(
            root->edge,
            root->left,
            insert(root->right, edge, x)
        ));
    }

    static std::size_t minimum_edge(Root root){
        while(root->left != nullptr) root = root->left;
        return root->edge;
    }

    Root erase(
        Root root,
        std::size_t edge,
        long double x
    ) const{
        if(root == nullptr) return nullptr;
        if(root->edge == edge){
            if(root->left == nullptr) return root->right;
            if(root->right == nullptr) return root->left;
            const std::size_t successor = minimum_edge(root->right);
            return balance(node(
                successor,
                root->left,
                erase(root->right, successor, x)
            ));
        }
        if(less(edge, root->edge, x)){
            return balance(node(
                root->edge,
                erase(root->left, edge, x),
                root->right
            ));
        }
        return balance(node(
            root->edge,
            root->left,
            erase(root->right, edge, x)
        ));
    }
    std::optional<std::size_t> event_boundary(
        const Point& point
    ) const{
        const auto vertical = vertical_.find(point.x);
        if(vertical != vertical_.end()){
            const auto iterator = std::upper_bound(
                vertical->second.begin(),
                vertical->second.end(),
                point.y,
                [](long double value, const VerticalRecord& record){
                    return value < record.lower;
                }
            );
            if(iterator != vertical->second.begin()){
                const VerticalRecord& record = *std::prev(iterator);
                if(point.y <= record.prefix_upper){
                    return record.prefix_face;
                }
            }
        }
        const auto endpoint = endpoints_.find(point.x);
        if(endpoint == endpoints_.end()) return std::nullopt;
        const auto iterator = std::lower_bound(
            endpoint->second.begin(),
            endpoint->second.end(),
            std::pair{point.y, std::size_t{0}}
        );
        if(iterator != endpoint->second.end()
            && iterator->first == point.y){
            return iterator->second;
        }
        return std::nullopt;
    }

    void build_versions(){
        if(events_.empty()) return;
        std::vector<std::vector<std::size_t>> starts(events_.size());
        std::vector<std::vector<std::size_t>> ends(events_.size());
        for(std::size_t edge = 0; edge < edges_.size(); ++edge){
            const long double minimum_x = std::min(
                edges_[edge].first.x, edges_[edge].second.x
            );
            const long double maximum_x = std::max(
                edges_[edge].first.x, edges_[edge].second.x
            );
            if(minimum_x == maximum_x) continue;
            const std::size_t first = static_cast<std::size_t>(
                std::lower_bound(
                    events_.begin(), events_.end(), minimum_x
                ) - events_.begin()
            );
            const std::size_t last = static_cast<std::size_t>(
                std::lower_bound(
                    events_.begin(), events_.end(), maximum_x
                ) - events_.begin()
            );
            starts[first].push_back(edge);
            ends[last].push_back(edge);
        }
        roots_.assign(events_.size() + 1, nullptr);
        Root root;
        for(std::size_t event = 0; event < events_.size(); ++event){
            const long double left_sample = event == 0
                ? events_[event] - 1.0L
                : (events_[event - 1] + events_[event]) / 2.0L;
            for(std::size_t edge: ends[event]){
                root = erase(root, edge, left_sample);
            }
            const long double right_sample =
                event + 1 == events_.size()
                ? events_[event] + 1.0L
                : (events_[event] + events_[event + 1]) / 2.0L;
            for(std::size_t edge: starts[event]){
                root = insert(root, edge, right_sample);
            }
            roots_[event + 1] = root;
        }
    }

    std::vector<std::vector<Point>> faces_;
    std::vector<EdgeRecord> edges_;
    std::vector<std::size_t> edge_indices_;
    std::vector<long double> events_;
    std::vector<Root> roots_;
    std::map<
        long double,
        std::vector<std::pair<long double, std::size_t>>
    > endpoints_;
    std::map<long double, std::vector<VerticalRecord>> vertical_;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_PLANAR_POINT_LOCATION_HPP_INCLUDED
