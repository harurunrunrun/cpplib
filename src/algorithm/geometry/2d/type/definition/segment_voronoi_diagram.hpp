#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_SEGMENT_VORONOI_DIAGRAM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_SEGMENT_VORONOI_DIAGRAM_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"

enum class SegmentVoronoiFeatureKind{
    first_endpoint,
    interior,
    second_endpoint,
};

enum class SegmentVoronoiCurveKind{
    line,
    parabola,
};

struct SegmentVoronoiFeature{
    std::size_t segment = 0;
    SegmentVoronoiFeatureKind kind =
        SegmentVoronoiFeatureKind::interior;
};

struct SegmentVoronoiCurve{
    SegmentVoronoiCurveKind kind = SegmentVoronoiCurveKind::line;
    SegmentVoronoiFeature first_feature;
    SegmentVoronoiFeature second_feature;
    Point origin;
    Point tangent;
    Point normal;
    long double focus_height = 0.0L;
    // A*x^2 + B*x*y + C*y^2 + D*x + E*y + F = 0.
    std::array<long double, 6> implicit{};

    Point point(long double parameter) const{
        if(kind == SegmentVoronoiCurveKind::line){
            return origin + tangent * parameter;
        }
        return origin + tangent * parameter + normal * (
            (parameter * parameter
                + focus_height * focus_height)
            / (2.0L * focus_height)
        );
    }

    long double parameter(const Point& query) const{
        return dot(query - origin, tangent);
    }
};

struct SegmentVoronoiEdge{
    static constexpr std::size_t no_vertex =
        std::numeric_limits<std::size_t>::max();
    std::size_t curve = 0;
    std::size_t first_vertex = no_vertex;
    std::size_t second_vertex = no_vertex;
    long double first_parameter =
        -std::numeric_limits<long double>::infinity();
    long double second_parameter =
        std::numeric_limits<long double>::infinity();
};

struct SegmentVoronoiLocation{
    static constexpr std::size_t no_site =
        std::numeric_limits<std::size_t>::max();
    std::size_t segment = no_site;
    Point closest_point{};
    long double distance =
        std::numeric_limits<long double>::infinity();
};

namespace segment_voronoi_detail{

inline long double polynomial_value(
    const std::vector<long double>& coefficients,
    long double argument
){
    long double result = 0.0L;
    for(auto iterator = coefficients.rbegin();
        iterator != coefficients.rend();
        ++iterator){
        result = result * argument + *iterator;
    }
    return result;
}

inline std::vector<long double> real_roots(
    std::vector<long double> coefficients
){
    while(coefficients.size() > 1
        && plus_geometry_detail::sign(coefficients.back()) == 0){
        coefficients.pop_back();
    }
    const std::size_t degree = coefficients.size() - 1;
    if(degree == 0) return {};
    if(degree == 1) return {-coefficients[0] / coefficients[1]};
    std::vector<long double> derivative(degree);
    for(std::size_t index = 1; index < coefficients.size(); ++index){
        derivative[index - 1] =
            coefficients[index] * static_cast<long double>(index);
    }
    std::vector<long double> critical = real_roots(std::move(derivative));
    const long double leading = std::abs(coefficients.back());
    long double bound = 1.0L;
    for(std::size_t index = 0; index < degree; ++index){
        bound = std::max(
            bound,
            1.0L + std::abs(coefficients[index]) / leading
        );
    }
    critical.erase(
        std::remove_if(
            critical.begin(), critical.end(),
            [&](long double value){
                return value <= -bound || value >= bound;
            }
        ),
        critical.end()
    );
    std::vector<long double> cuts{-bound};
    cuts.insert(cuts.end(), critical.begin(), critical.end());
    cuts.push_back(bound);
    std::vector<long double> roots;
    const long double coefficient_scale = std::max(
        1.0L,
        std::abs(*std::max_element(
            coefficients.begin(), coefficients.end(),
            [](long double left, long double right){
                return std::abs(left) < std::abs(right);
            }
        ))
    );
    const auto zero = [&](long double value){
        return std::abs(value)
            <= 4096.0L * std::numeric_limits<long double>::epsilon()
                * std::abs(coefficient_scale);
    };
    for(long double point: critical){
        if(zero(polynomial_value(coefficients, point))){
            roots.push_back(point);
        }
    }
    for(std::size_t interval = 1; interval < cuts.size(); ++interval){
        long double left = cuts[interval - 1];
        long double right = cuts[interval];
        long double left_value =
            polynomial_value(coefficients, left);
        const long double right_value =
            polynomial_value(coefficients, right);
        if(zero(left_value)) roots.push_back(left);
        if((left_value < 0.0L) == (right_value < 0.0L)) continue;
        for(int iteration = 0; iteration < 100; ++iteration){
            const long double middle = (left + right) / 2.0L;
            const long double middle_value =
                polynomial_value(coefficients, middle);
            if((left_value < 0.0L) == (middle_value < 0.0L)){
                left = middle;
                left_value = middle_value;
            }else{
                right = middle;
            }
        }
        roots.push_back((left + right) / 2.0L);
    }
    std::sort(roots.begin(), roots.end());
    roots.erase(std::unique(
        roots.begin(), roots.end(),
        [](long double left, long double right){
            return plus_geometry_detail::sign(
                left - right,
                std::max({1.0L, std::abs(left), std::abs(right)})
            ) == 0;
        }
    ), roots.end());
    return roots;
}

inline std::vector<long double> substitute(
    const SegmentVoronoiCurve& parameterized,
    const std::array<long double, 6>& implicit
){
    const long double inverse_height =
        parameterized.kind == SegmentVoronoiCurveKind::parabola
        ? 1.0L / (2.0L * parameterized.focus_height)
        : 0.0L;
    const std::array<long double, 3> x{{
        parameterized.origin.x
            + parameterized.normal.x
                * parameterized.focus_height / 2.0L,
        parameterized.tangent.x,
        parameterized.normal.x * inverse_height,
    }};
    const std::array<long double, 3> y{{
        parameterized.origin.y
            + parameterized.normal.y
                * parameterized.focus_height / 2.0L,
        parameterized.tangent.y,
        parameterized.normal.y * inverse_height,
    }};
    std::vector<long double> result(5, 0.0L);
    const auto add_product = [&](const std::array<long double, 3>& first,
                                 const std::array<long double, 3>& second,
                                 long double scale){
        for(std::size_t left = 0; left < 3; ++left){
            for(std::size_t right = 0; right < 3; ++right){
                result[left + right] +=
                    scale * first[left] * second[right];
            }
        }
    };
    add_product(x, x, implicit[0]);
    add_product(x, y, implicit[1]);
    add_product(y, y, implicit[2]);
    for(std::size_t degree = 0; degree < 3; ++degree){
        result[degree] +=
            implicit[3] * x[degree] + implicit[4] * y[degree];
    }
    result[0] += implicit[5];
    return result;
}

}  // namespace segment_voronoi_detail

class SegmentVoronoiDiagram{
public:
    SegmentVoronoiDiagram() = default;

    explicit SegmentVoronoiDiagram(std::vector<Segment> segments)
        : segments_(std::move(segments)),
          cell_edges_(segments_.size()){
        for(const Segment& segment: segments_){
            plus_geometry_detail::validate_finite(segment.a);
            plus_geometry_detail::validate_finite(segment.b);
        }
        for(std::size_t first = 0; first < segments_.size(); ++first){
            for(std::size_t second = first + 1;
                second < segments_.size();
                ++second){
                const Segment& a = segments_[first];
                const Segment& b = segments_[second];
                const bool same = plus_geometry_detail::close(a.a, b.a)
                    && plus_geometry_detail::close(a.b, b.b);
                const bool reversed = plus_geometry_detail::close(a.a, b.b)
                    && plus_geometry_detail::close(a.b, b.a);
                if(same || reversed){
                    throw std::invalid_argument(
                        "segment Voronoi sites must be geometrically distinct"
                    );
                }
                if(plus_geometry_detail::orientation(a.a, a.b, b.a) != 0
                    || plus_geometry_detail::orientation(a.a, a.b, b.b) != 0){
                    continue;
                }
                const Point direction = a.b - a.a;
                const bool use_x = std::abs(direction.x)
                    >= std::abs(direction.y);
                const auto coordinate = [&](const Point& point){
                    return use_x ? point.x : point.y;
                };
                const long double overlap = std::min(
                    std::max(coordinate(a.a), coordinate(a.b)),
                    std::max(coordinate(b.a), coordinate(b.b))
                ) - std::max(
                    std::min(coordinate(a.a), coordinate(a.b)),
                    std::min(coordinate(b.a), coordinate(b.b))
                );
                if(plus_geometry_detail::sign(
                    overlap,
                    std::max({
                        1.0L,
                        std::abs(coordinate(a.a)),
                        std::abs(coordinate(a.b)),
                        std::abs(coordinate(b.a)),
                        std::abs(coordinate(b.b)),
                    })
                ) > 0){
                    throw std::invalid_argument(
                        "collinear segment Voronoi sites must not overlap"
                    );
                }
            }
        }
        build_curves();
        build_edges();
        compact_vertices();
    }

    SegmentVoronoiLocation locate(const Point& query) const{
        plus_geometry_detail::validate_finite(query);
        SegmentVoronoiLocation result;
        for(std::size_t segment = 0;
            segment < segments_.size();
            ++segment){
            const auto [closest, distance] =
                closest_point_and_distance(segment, query);
            if(result.segment == SegmentVoronoiLocation::no_site
                || distance < result.distance
                || (distance == result.distance
                    && segment < result.segment)){
                result = {segment, closest, distance};
            }
        }
        return result;
    }

    bool on_edge(std::size_t edge, const Point& point) const{
        if(edge >= edges_.size()){
            throw std::out_of_range(
                "segment Voronoi edge index is out of range"
            );
        }
        plus_geometry_detail::validate_finite(point);

        const SegmentVoronoiEdge& current = edges_[edge];
        if((current.first_vertex != SegmentVoronoiEdge::no_vertex
                && plus_geometry_detail::close(
                    vertices_[current.first_vertex], point
                ))
            || (current.second_vertex != SegmentVoronoiEdge::no_vertex
                && plus_geometry_detail::close(
                    vertices_[current.second_vertex], point
                ))){
            return true;
        }
        const long double parameter =
            curves_[current.curve].parameter(point);
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
            && on_curve(current.curve, point);
    }

    const std::vector<Segment>& sites() const{ return segments_; }
    const std::vector<SegmentVoronoiCurve>& curves() const{
        return curves_;
    }
    const std::vector<Point>& vertices() const{ return vertices_; }
    const std::vector<SegmentVoronoiEdge>& edges() const{
        return edges_;
    }
    const std::vector<std::vector<std::size_t>>& cell_edges() const{
        return cell_edges_;
    }

private:
    Point endpoint(const SegmentVoronoiFeature& feature) const{
        return feature.kind == SegmentVoronoiFeatureKind::first_endpoint
            ? segments_[feature.segment].a
            : segments_[feature.segment].b;
    }

    bool feature_active(
        const SegmentVoronoiFeature& feature,
        const Point& point
    ) const{
        const Segment& segment = segments_[feature.segment];
        const Point direction = segment.b - segment.a;
        const long double squared_length = dot(direction, direction);
        if(squared_length == 0.0L){
            return feature.kind
                == SegmentVoronoiFeatureKind::first_endpoint;
        }
        const long double projection =
            dot(point - segment.a, direction) / squared_length;
        if(feature.kind == SegmentVoronoiFeatureKind::first_endpoint){
            return plus_geometry_detail::sign(projection) <= 0;
        }
        if(feature.kind == SegmentVoronoiFeatureKind::second_endpoint){
            return plus_geometry_detail::sign(projection - 1.0L) >= 0;
        }
        return plus_geometry_detail::sign(projection) >= 0
            && plus_geometry_detail::sign(projection - 1.0L) <= 0;
    }

    long double feature_distance(
        const SegmentVoronoiFeature& feature,
        const Point& point
    ) const{
        if(feature.kind != SegmentVoronoiFeatureKind::interior){
            return std::sqrt(plus_geometry_detail::squared_distance(
                endpoint(feature), point
            ));
        }
        const Segment& segment = segments_[feature.segment];
        const Point direction = segment.b - segment.a;
        return std::abs(cross(direction, point - segment.a))
            / std::sqrt(dot(direction, direction));
    }

    std::pair<Point, long double> closest_point_and_distance(
        std::size_t index,
        const Point& query
    ) const{
        const Segment& segment = segments_[index];
        const Point direction = segment.b - segment.a;
        const long double denominator = dot(direction, direction);
        const long double parameter = denominator == 0.0L ? 0.0L
            : std::clamp(
                dot(query - segment.a, direction) / denominator,
                0.0L,
                1.0L
            );
        const Point closest = segment.a + direction * parameter;
        return {
            closest,
            std::sqrt(
                plus_geometry_detail::squared_distance(query, closest)
            ),
        };
    }

    bool on_curve(std::size_t curve, const Point& point) const{
        const SegmentVoronoiCurve& current = curves_[curve];
        if(!feature_active(current.first_feature, point)
            || !feature_active(current.second_feature, point)){
            return false;
        }
        const long double first =
            feature_distance(current.first_feature, point);
        const long double second =
            feature_distance(current.second_feature, point);
        if(plus_geometry_detail::sign(
            first - second,
            std::max({1.0L, first, second})
        ) != 0){
            return false;
        }
        for(std::size_t segment = 0;
            segment < segments_.size();
            ++segment){
            if(plus_geometry_detail::sign(
                closest_point_and_distance(segment, point).second - first,
                std::max(1.0L, first)
            ) < 0){
                return false;
            }
        }
        return true;
    }

    void append_curve(SegmentVoronoiCurve curve){
        curves_.push_back(std::move(curve));
    }

    void add_point_point(
        SegmentVoronoiFeature first,
        SegmentVoronoiFeature second
    ){
        const Point a = endpoint(first);
        const Point b = endpoint(second);
        const Point difference = b - a;
        const long double length = std::sqrt(dot(difference, difference));
        if(length == 0.0L) return;
        const Point normal = difference / length;
        const Point origin = (a + b) / 2.0L;
        append_curve({
            SegmentVoronoiCurveKind::line, first, second,
            origin, {-normal.y, normal.x}, normal, 0.0L,
            {{0.0L, 0.0L, 0.0L,
              normal.x, normal.y, -dot(normal, origin)}},
        });
    }

    void add_point_line(
        SegmentVoronoiFeature point_feature,
        SegmentVoronoiFeature line_feature,
        bool reversed
    ){
        const Point focus = endpoint(point_feature);
        const Segment& line = segments_[line_feature.segment];
        Point tangent = line.b - line.a;
        tangent /= std::sqrt(dot(tangent, tangent));
        Point normal{-tangent.y, tangent.x};
        long double height = dot(focus - line.a, normal);
        if(height < 0.0L){
            height = -height;
            normal = -normal;
        }
        const auto first = reversed ? line_feature : point_feature;
        const auto second = reversed ? point_feature : line_feature;
        if(plus_geometry_detail::sign(height) == 0){
            append_curve({
                SegmentVoronoiCurveKind::line, first, second,
                focus, normal, tangent, 0.0L,
                {{0.0L, 0.0L, 0.0L,
                  tangent.x, tangent.y, -dot(tangent, focus)}},
            });
            return;
        }
        const Point origin = focus - normal * height;
        const long double constant = -dot(normal, origin);
        append_curve({
            SegmentVoronoiCurveKind::parabola, first, second,
            origin, tangent, normal, height,
            {{
                1.0L - normal.x * normal.x,
                -2.0L * normal.x * normal.y,
                1.0L - normal.y * normal.y,
                -2.0L * focus.x
                    - 2.0L * constant * normal.x,
                -2.0L * focus.y
                    - 2.0L * constant * normal.y,
                dot(focus, focus) - constant * constant,
            }},
        });
    }

    void add_line_line(
        SegmentVoronoiFeature first,
        SegmentVoronoiFeature second
    ){
        const Segment& a = segments_[first.segment];
        const Segment& b = segments_[second.segment];
        Point first_direction = a.b - a.a;
        Point second_direction = b.b - b.a;
        first_direction /= std::sqrt(dot(first_direction, first_direction));
        second_direction /= std::sqrt(dot(second_direction, second_direction));
        const Point first_normal{-first_direction.y, first_direction.x};
        const Point second_normal{-second_direction.y, second_direction.x};
        const long double first_constant = -dot(first_normal, a.a);
        const long double second_constant = -dot(second_normal, b.a);
        for(const int sign: {-1, 1}){
            const Point normal = first_normal
                + second_normal * static_cast<long double>(sign);
            const long double norm = dot(normal, normal);
            if(plus_geometry_detail::sign(norm) == 0) continue;
            const long double constant = first_constant
                + second_constant * static_cast<long double>(sign);
            const Point origin = normal * (-constant / norm);
            const Point unit_normal = normal / std::sqrt(norm);
            append_curve({
                SegmentVoronoiCurveKind::line, first, second,
                origin, {-unit_normal.y, unit_normal.x}, unit_normal,
                0.0L,
                {{0.0L, 0.0L, 0.0L,
                  unit_normal.x, unit_normal.y,
                  -dot(unit_normal, origin)}},
            });
        }
    }

    void build_curves(){
        std::vector<SegmentVoronoiFeature> features;
        for(std::size_t segment = 0;
            segment < segments_.size();
            ++segment){
            features.push_back({
                segment, SegmentVoronoiFeatureKind::first_endpoint
            });
            if(plus_geometry_detail::close(
                segments_[segment].a, segments_[segment].b
            )) continue;
            features.push_back({
                segment, SegmentVoronoiFeatureKind::interior
            });
            features.push_back({
                segment, SegmentVoronoiFeatureKind::second_endpoint
            });
        }
        for(std::size_t first = 0; first < features.size(); ++first){
            for(std::size_t second = first + 1;
                second < features.size();
                ++second){
                if(features[first].segment
                    == features[second].segment){
                    continue;
                }
                const bool first_line = features[first].kind
                    == SegmentVoronoiFeatureKind::interior;
                const bool second_line = features[second].kind
                    == SegmentVoronoiFeatureKind::interior;
                if(!first_line && !second_line){
                    add_point_point(features[first], features[second]);
                }else if(!first_line){
                    add_point_line(features[first], features[second], false);
                }else if(!second_line){
                    add_point_line(features[second], features[first], true);
                }else{
                    add_line_line(features[first], features[second]);
                }
            }
        }
    }

    static bool share_site(
        const SegmentVoronoiCurve& first,
        const SegmentVoronoiCurve& second
    ){
        const std::size_t first_sites[] = {
            first.first_feature.segment,
            first.second_feature.segment,
        };
        const std::size_t second_sites[] = {
            second.first_feature.segment,
            second.second_feature.segment,
        };
        for(std::size_t left: first_sites){
            for(std::size_t right: second_sites){
                if(left == right) return true;
            }
        }
        return false;
    }

    std::size_t vertex_id(const Point& point){
        vertices_.push_back(point);
        return vertices_.size() - 1;
    }

    void add_feature_events(
        std::size_t curve,
        std::vector<std::pair<long double, std::size_t>>& events
    ){
        for(const SegmentVoronoiFeature feature: {
            curves_[curve].first_feature,
            curves_[curve].second_feature,
        }){
            const Segment& segment = segments_[feature.segment];
            const Point direction = segment.b - segment.a;
            if(dot(direction, direction) == 0.0L) continue;
            for(const Point endpoint_point: {segment.a, segment.b}){
                const Point normal = direction;
                const std::array<long double, 6> boundary{{
                    0.0L, 0.0L, 0.0L,
                    normal.x, normal.y, -dot(normal, endpoint_point),
                }};
                for(long double root:
                    segment_voronoi_detail::real_roots(
                        segment_voronoi_detail::substitute(
                            curves_[curve], boundary
                        )
                    )){
                    const Point point = curves_[curve].point(root);
                    events.push_back({root, vertex_id(point)});
                }
            }
        }
    }

    void append_active_edge(SegmentVoronoiEdge edge){
        if(std::isfinite(edge.first_parameter)
            && std::isfinite(edge.second_parameter)
            && plus_geometry_detail::sign(
                edge.second_parameter - edge.first_parameter,
                std::max({
                    1.0L,
                    std::abs(edge.first_parameter),
                    std::abs(edge.second_parameter),
                })
            ) <= 0){
            return;
        }
        const std::size_t index = edges_.size();
        edges_.push_back(edge);
        cell_edges_[
            curves_[edge.curve].first_feature.segment
        ].push_back(index);
        cell_edges_[
            curves_[edge.curve].second_feature.segment
        ].push_back(index);
    }

    void compact_vertices(){
        const std::size_t no_vertex = SegmentVoronoiEdge::no_vertex;
        std::vector<std::size_t> remap(vertices_.size(), no_vertex);
        std::vector<std::size_t> used;
        for(const SegmentVoronoiEdge& edge: edges_){
            if(edge.first_vertex != no_vertex){
                used.push_back(edge.first_vertex);
            }
            if(edge.second_vertex != no_vertex){
                used.push_back(edge.second_vertex);
            }
        }
        std::sort(used.begin(), used.end(), [&](std::size_t left,
                                                 std::size_t right){
            if(vertices_[left] < vertices_[right]) return true;
            if(vertices_[right] < vertices_[left]) return false;
            return left < right;
        });
        used.erase(std::unique(used.begin(), used.end()), used.end());
        std::vector<Point> compact;
        for(std::size_t old: used){
            if(compact.empty()
                || !plus_geometry_detail::close(
                    compact.back(), vertices_[old]
                )){
                compact.push_back(vertices_[old]);
            }
            remap[old] = compact.size() - 1;
        }
        for(SegmentVoronoiEdge& edge: edges_){
            if(edge.first_vertex != no_vertex){
                edge.first_vertex = remap[edge.first_vertex];
            }
            if(edge.second_vertex != no_vertex){
                edge.second_vertex = remap[edge.second_vertex];
            }
        }
        vertices_ = std::move(compact);
    }

    void build_edges(){
        std::vector<
            std::vector<std::pair<long double, std::size_t>>
        > events(curves_.size());
        for(std::size_t curve = 0; curve < curves_.size(); ++curve){
            add_feature_events(curve, events[curve]);
        }
        for(std::size_t first = 0; first < curves_.size(); ++first){
            for(std::size_t second = first + 1;
                second < curves_.size();
                ++second){
                if(!share_site(curves_[first], curves_[second])) continue;
                const auto roots = segment_voronoi_detail::real_roots(
                    segment_voronoi_detail::substitute(
                        curves_[first], curves_[second].implicit
                    )
                );
                for(long double parameter: roots){
                    const Point point = curves_[first].point(parameter);
                    const long double second_parameter =
                        curves_[second].parameter(point);
                    if(!plus_geometry_detail::close(
                        point, curves_[second].point(second_parameter)
                    )) continue;
                    const std::size_t vertex = vertex_id(point);
                    events[first].push_back({parameter, vertex});
                    events[second].push_back({
                        second_parameter, vertex
                    });
                }
            }
        }
        for(std::size_t curve = 0; curve < curves_.size(); ++curve){
            auto& current = events[curve];
            std::sort(current.begin(), current.end());
            current.erase(std::unique(
                current.begin(), current.end(),
                [](const auto& left, const auto& right){
                    return plus_geometry_detail::sign(
                        left.first - right.first,
                        std::max({
                            1.0L,
                            std::abs(left.first),
                            std::abs(right.first),
                        })
                    ) == 0;
                }
            ), current.end());
            std::vector<long double> cuts{
                -std::numeric_limits<long double>::infinity()
            };
            for(const auto& event: current) cuts.push_back(event.first);
            cuts.push_back(std::numeric_limits<long double>::infinity());
            bool in_run = false;
            SegmentVoronoiEdge run;
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
                const bool active = on_curve(
                    curve, curves_[curve].point(sample)
                );
                if(active){
                    if(!in_run){
                        run = {
                            curve,
                            interval >= 2
                                ? current[interval - 2].second
                                : SegmentVoronoiEdge::no_vertex,
                            SegmentVoronoiEdge::no_vertex,
                            cuts[interval - 1],
                            cuts[interval],
                        };
                        in_run = true;
                    }
                    run.second_parameter = cuts[interval];
                    run.second_vertex = interval - 1 < current.size()
                        ? current[interval - 1].second
                        : SegmentVoronoiEdge::no_vertex;
                }else if(in_run){
                    append_active_edge(run);
                    in_run = false;
                }
            }
            if(in_run) append_active_edge(run);
        }
    }
    std::vector<Segment> segments_;
    std::vector<SegmentVoronoiCurve> curves_;
    std::vector<Point> vertices_;
    std::vector<SegmentVoronoiEdge> edges_;
    std::vector<std::vector<std::size_t>> cell_edges_;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_SEGMENT_VORONOI_DIAGRAM_HPP_INCLUDED
