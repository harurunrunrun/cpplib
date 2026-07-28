#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_POLYGON_MEDIAL_AXIS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_POLYGON_MEDIAL_AXIS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "segment_voronoi_diagram.hpp"

struct MedialAxisLocation{
    long double radius = 0.0L;
    std::vector<std::size_t> nearest_edges;
    std::vector<Point> nearest_points;
};

struct MedialAxisBranch{
    std::size_t curve = 0;
    std::size_t first_boundary_edge = 0;
    std::size_t second_boundary_edge = 0;
    long double first_parameter =
        -std::numeric_limits<long double>::infinity();
    long double second_parameter =
        std::numeric_limits<long double>::infinity();
};

class PolygonMedialAxis{
public:
    explicit PolygonMedialAxis(std::vector<Point> polygon)
        : polygon_(prepare_polygon(std::move(polygon))),
          diagram_(boundary_segments(polygon_)){
        build_branches();
    }

    MedialAxisLocation locate(const Point& query) const{
        plus_geometry_detail::validate_finite(query);
        MedialAxisLocation result;
        result.radius = std::numeric_limits<long double>::infinity();
        std::vector<long double> distances(polygon_.size());
        std::vector<Point> closest_points(polygon_.size());
        std::vector<std::size_t> closest_features(polygon_.size());
        for(std::size_t edge = 0; edge < polygon_.size(); ++edge){
            const Segment segment{
                polygon_[edge],
                polygon_[(edge + 1) % polygon_.size()],
            };
            const Point direction = segment.b - segment.a;
            const long double squared_length = dot(direction, direction);
            const long double parameter = std::clamp(
                dot(query - segment.a, direction) / squared_length,
                0.0L,
                1.0L
            );
            closest_points[edge] = segment.a + direction * parameter;
            if(plus_geometry_detail::sign(parameter) == 0){
                closest_features[edge] = edge;
            }else if(plus_geometry_detail::sign(parameter - 1.0L) == 0){
                closest_features[edge] = (edge + 1) % polygon_.size();
            }else{
                closest_features[edge] = polygon_.size() + edge;
            }
            distances[edge] = std::sqrt(
                plus_geometry_detail::squared_distance(
                    query, closest_points[edge]
                )
            );
            result.radius = std::min(result.radius, distances[edge]);
        }
        std::vector<bool> reported(2 * polygon_.size(), false);
        for(std::size_t edge = 0; edge < distances.size(); ++edge){
            if(plus_geometry_detail::sign(
                distances[edge] - result.radius,
                std::max(1.0L, result.radius)
            ) != 0){
                continue;
            }
            result.nearest_edges.push_back(edge);
            if(!reported[closest_features[edge]]){
                reported[closest_features[edge]] = true;
                result.nearest_points.push_back(closest_points[edge]);
            }
        }
        return result;
    }

    bool on_axis(const Point& query) const{
        return plus_geometry_detail::point_in_polygon(polygon_, query) == 2
            && locate(query).nearest_points.size() >= 2;
    }

    bool contains(const Point& query) const{ return on_axis(query); }

    bool on_branch(std::size_t branch, const Point& query) const{
        if(branch >= branches_.size()){
            throw std::out_of_range("medial-axis branch is out of range");
        }
        plus_geometry_detail::validate_finite(query);

        const MedialAxisBranch& current = branches_[branch];
        const std::size_t source_edge = source_edges_[branch];
        const long double parameter =
            diagram_.curves()[current.curve].parameter(query);
        const long double parameter_scale = std::max({
            1.0L,
            std::abs(parameter),
            std::abs(current.first_parameter),
            std::abs(current.second_parameter),
        });
        return plus_geometry_detail::sign(
                parameter - current.first_parameter, parameter_scale
            ) >= 0
            && plus_geometry_detail::sign(
                parameter - current.second_parameter, parameter_scale
            ) <= 0
            && diagram_.on_edge(source_edge, query);
    }

    const std::vector<Point>& polygon() const{ return polygon_; }
    const std::vector<SegmentVoronoiCurve>& curves() const{
        return diagram_.curves();
    }
    const std::vector<MedialAxisBranch>& branches() const{
        return branches_;
    }

private:
    static std::vector<Point> prepare_polygon(
        std::vector<Point> polygon
    ){
        polygon = plus_geometry_detail::normalized_polygon(
            std::move(polygon)
        );
        if(polygon.size() < 3){
            throw std::invalid_argument(
                "a medial axis requires a nondegenerate polygon"
            );
        }
        for(const Point& point: polygon){
            plus_geometry_detail::validate_finite(point);
        }
        if(plus_geometry_detail::sign(
            plus_geometry_detail::signed_area_twice(polygon)
        ) == 0){
            throw std::invalid_argument(
                "a medial-axis polygon must have positive area"
            );
        }
        return polygon;
    }

    static std::vector<Segment> boundary_segments(
        const std::vector<Point>& polygon
    ){
        std::vector<Segment> result;
        result.reserve(polygon.size());
        for(std::size_t index = 0; index < polygon.size(); ++index){
            result.push_back({
                polygon[index],
                polygon[(index + 1) % polygon.size()],
            });
        }
        return result;
    }

    void build_branches(){
        for(std::size_t source = 0;
            source < diagram_.edges().size();
            ++source){
            const SegmentVoronoiEdge& edge = diagram_.edges()[source];
            const SegmentVoronoiCurve& curve =
                diagram_.curves()[edge.curve];
            std::vector<long double> cuts{
                edge.first_parameter,
                edge.second_parameter,
            };
            for(std::size_t index = 0; index < polygon_.size(); ++index){
                const Point first = polygon_[index];
                const Point second =
                    polygon_[(index + 1) % polygon_.size()];
                const Point direction = second - first;
                const Point normal{-direction.y, direction.x};
                const std::array<long double, 6> boundary{{
                    0.0L, 0.0L, 0.0L,
                    normal.x, normal.y, -dot(normal, first),
                }};
                for(long double parameter:
                    segment_voronoi_detail::real_roots(
                        segment_voronoi_detail::substitute(
                            curve, boundary
                        )
                    )){
                    if(parameter <= edge.first_parameter
                        || edge.second_parameter <= parameter){
                        continue;
                    }
                    const Point point = curve.point(parameter);
                    if(plus_geometry_detail::sign(
                        dot(point - first, point - second),
                        plus_geometry_detail::squared_distance(
                            first, second
                        )
                    ) <= 0){
                        cuts.push_back(parameter);
                    }
                }
            }
            std::sort(cuts.begin(), cuts.end());
            cuts.erase(std::unique(
                cuts.begin(), cuts.end(),
                [](long double left, long double right){
                    return plus_geometry_detail::sign(
                        left - right,
                        std::max({
                            1.0L, std::abs(left), std::abs(right)
                        })
                    ) == 0;
                }
            ), cuts.end());
            for(std::size_t interval = 1;
                interval < cuts.size();
                ++interval){
                long double sample;
                if(std::isinf(cuts[interval - 1])
                    && std::isinf(cuts[interval])){
                    sample = 0.0L;
                }else if(std::isinf(cuts[interval - 1])){
                    sample = cuts[interval]
                        - std::max(1.0L, std::abs(cuts[interval]));
                }else if(std::isinf(cuts[interval])){
                    sample = cuts[interval - 1]
                        + std::max(1.0L, std::abs(cuts[interval - 1]));
                }else{
                    sample =
                        (cuts[interval - 1] + cuts[interval]) / 2.0L;
                }
                if(on_axis(curve.point(sample))){
                    branches_.push_back({
                        edge.curve,
                        curve.first_feature.segment,
                        curve.second_feature.segment,
                        cuts[interval - 1],
                        cuts[interval],
                    });
                    source_edges_.push_back(source);
                }
            }
        }
    }

    std::vector<Point> polygon_;
    SegmentVoronoiDiagram diagram_;
    std::vector<MedialAxisBranch> branches_;
    std::vector<std::size_t> source_edges_;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_POLYGON_MEDIAL_AXIS_HPP_INCLUDED
