#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_APOLLONIUS_DIAGRAM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_APOLLONIUS_DIAGRAM_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "weighted_point.hpp"

enum class ApolloniusCurveKind{
    line,
    hyperbola,
    ray,
};

struct ApolloniusCurve{
    ApolloniusCurveKind kind = ApolloniusCurveKind::line;
    std::size_t first_site = 0;
    std::size_t second_site = 0;
    Point origin;
    Point axis;
    Point transverse;
    long double first_radius = 0.0L;
    long double second_radius = 0.0L;
    int branch = 0;

    Point point(long double parameter) const{
        if(kind == ApolloniusCurveKind::line){
            return origin + axis * parameter;
        }
        if(kind == ApolloniusCurveKind::ray){
            return origin + axis * std::max(0.0L, parameter);
        }
        return origin
            + axis * (
                static_cast<long double>(branch)
                * first_radius * std::cosh(parameter)
            )
            + transverse * (second_radius * std::sinh(parameter));
    }

    long double parameter(const Point& query) const{
        if(kind != ApolloniusCurveKind::hyperbola){
            return dot(query - origin, axis);
        }
        return std::asinh(
            dot(query - origin, transverse) / second_radius
        );
    }
};

struct ApolloniusEdge{
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

struct ApolloniusLocation{
    static constexpr std::size_t no_site =
        std::numeric_limits<std::size_t>::max();
    std::size_t site = no_site;
    long double weighted_distance =
        std::numeric_limits<long double>::infinity();
};

class ApolloniusDiagram{
public:
    ApolloniusDiagram() = default;

    explicit ApolloniusDiagram(std::vector<WeightedPoint> sites)
        : sites_(std::move(sites)),
          cell_edges_(sites_.size()),
          curve_index_(
              sites_.size() * sites_.size(),
              std::numeric_limits<std::size_t>::max()
          ){
        validate();
        build_curves();
        build_vertices();
        build_edges();
        compact_vertices();
    }

    ApolloniusLocation locate(const Point& query) const{
        plus_geometry_detail::validate_finite(query);
        ApolloniusLocation result;
        for(std::size_t site = 0; site < sites_.size(); ++site){
            const long double current = weighted_distance(site, query);
            if(result.site == ApolloniusLocation::no_site
                || current < result.weighted_distance
                || (current == result.weighted_distance
                    && site < result.site)){
                result = {site, current};
            }
        }
        return result;
    }

    bool on_edge(std::size_t edge, const Point& point) const{
        if(edge >= edges_.size()){
            throw std::out_of_range("Apollonius edge index is out of range");
        }
        plus_geometry_detail::validate_finite(point);
        const ApolloniusEdge& current = edges_[edge];
        if((current.first_vertex != ApolloniusEdge::no_vertex
                && plus_geometry_detail::close(
                    vertices_[current.first_vertex], point
                ))
            || (current.second_vertex != ApolloniusEdge::no_vertex
                && plus_geometry_detail::close(
                    vertices_[current.second_vertex], point
                ))){
            return true;
        }
        const ApolloniusCurve& curve = curves_[current.curve];
        const long double parameter = curve.parameter(point);
        const long double parameter_scale = std::max({
            1.0L,
            std::abs(parameter),
            std::abs(current.first_parameter),
            std::abs(current.second_parameter),
        });
        if(plus_geometry_detail::sign(
                parameter - current.first_parameter, parameter_scale
            ) < 0
            || plus_geometry_detail::sign(
                parameter - current.second_parameter, parameter_scale
            ) > 0){
            return false;
        }
        const long double first =
            weighted_distance(curve.first_site, point);
        const long double second =
            weighted_distance(curve.second_site, point);
        if(plus_geometry_detail::sign(
            first - second,
            std::max({1.0L, std::abs(first), std::abs(second)})
        ) != 0){
            return false;
        }
        for(std::size_t site = 0; site < sites_.size(); ++site){
            if(weighted_distance(site, point) < first
                && plus_geometry_detail::sign(
                    weighted_distance(site, point) - first,
                    std::max(1.0L, std::abs(first))
                ) < 0){
                return false;
            }
        }
        return true;
    }

    const std::vector<WeightedPoint>& sites() const{ return sites_; }
    const std::vector<ApolloniusCurve>& curves() const{ return curves_; }
    const std::vector<Point>& vertices() const{ return vertices_; }
    const std::vector<ApolloniusEdge>& edges() const{ return edges_; }
    const std::vector<std::vector<std::size_t>>& cell_edges() const{
        return cell_edges_;
    }

private:
    long double weighted_distance(
        std::size_t site,
        const Point& point
    ) const{
        return std::sqrt(plus_geometry_detail::squared_distance(
            sites_[site].point, point
        )) - sites_[site].weight;
    }

    void validate() const{
        for(const WeightedPoint& site: sites_){
            plus_geometry_detail::validate_finite(site.point);
            if(!std::isfinite(site.weight)){
                throw std::invalid_argument(
                    "Apollonius additive weights must be finite"
                );
            }
        }
    }

    void build_curves(){
        for(std::size_t first = 0; first < sites_.size(); ++first){
            for(std::size_t second = first + 1;
                second < sites_.size();
                ++second){
                const Point difference =
                    sites_[second].point - sites_[first].point;
                const long double distance =
                    std::sqrt(dot(difference, difference));
                const long double weight_difference =
                    sites_[first].weight - sites_[second].weight;
                if(distance == 0.0L){
                    if(weight_difference == 0.0L) continue;
                    continue;
                }
                const Point axis = difference / distance;
                const Point transverse{-axis.y, axis.x};
                const Point middle =
                    (sites_[first].point + sites_[second].point) / 2.0L;
                if(plus_geometry_detail::sign(weight_difference) == 0){
                    append_curve({
                        ApolloniusCurveKind::line,
                        first,
                        second,
                        middle,
                        transverse,
                        axis,
                        0.0L,
                        0.0L,
                        0,
                    });
                    continue;
                }
                const int comparison = plus_geometry_detail::sign(
                    std::abs(weight_difference) - distance,
                    distance
                );
                if(comparison > 0) continue;
                if(comparison == 0){
                    const int branch =
                        weight_difference > 0.0L ? 1 : -1;
                    append_curve({
                        ApolloniusCurveKind::ray,
                        first,
                        second,
                        branch > 0 ? sites_[second].point
                                   : sites_[first].point,
                        axis * static_cast<long double>(branch),
                        transverse,
                        0.0L,
                        0.0L,
                        branch,
                    });
                    continue;
                }
                const long double a =
                    std::abs(weight_difference) / 2.0L;
                const long double focal = distance / 2.0L;
                append_curve({
                    ApolloniusCurveKind::hyperbola,
                    first,
                    second,
                    middle,
                    axis,
                    transverse,
                    a,
                    std::sqrt(focal * focal - a * a),
                    weight_difference > 0.0L ? 1 : -1,
                });
            }
        }
    }

    void append_curve(ApolloniusCurve curve){
        const std::size_t index = curves_.size();
        curve_index_[curve.first_site * sites_.size() + curve.second_site] = index;
        curve_index_[curve.second_site * sites_.size() + curve.first_site] = index;
        curves_.push_back(std::move(curve));
        curve_events_.push_back({});
    }

    std::size_t consider_vertex(const Point& point){
        const std::size_t no_vertex = std::numeric_limits<std::size_t>::max();
        if(!std::isfinite(point.x) || !std::isfinite(point.y)){
            return no_vertex;
        }
        const ApolloniusLocation minimum = locate(point);
        std::size_t equal = 0;
        for(std::size_t site = 0; site < sites_.size(); ++site){
            if(plus_geometry_detail::sign(
                weighted_distance(site, point)
                    - minimum.weighted_distance,
                std::max(1.0L, std::abs(minimum.weighted_distance))
            ) == 0){
                ++equal;
            }
        }
        if(equal < 3) return no_vertex;
        vertices_.push_back(point);
        return vertices_.size() - 1;
    }

    void register_vertex(const Point& point, std::size_t first,
                         std::size_t second, std::size_t third){
        const std::size_t vertex = consider_vertex(point);
        if(vertex == std::numeric_limits<std::size_t>::max()) return;
        const std::pair<std::size_t, std::size_t> pairs[] = {
            {first, second}, {first, third}, {second, third}
        };
        for(const auto [left, right]: pairs){
            const std::size_t curve =
                curve_index_[left * sites_.size() + right];
            if(curve == std::numeric_limits<std::size_t>::max()) continue;
            curve_events_[curve].push_back({
                curves_[curve].parameter(point), vertex
            });
        }
    }

    void build_vertices(){
        for(std::size_t first = 0; first < sites_.size(); ++first){
            for(std::size_t second = first + 1;
                second < sites_.size();
                ++second){
                for(std::size_t third = second + 1;
                    third < sites_.size();
                    ++third){
                    const WeightedPoint& a = sites_[first];
                    const WeightedPoint& b = sites_[second];
                    const WeightedPoint& c = sites_[third];
                    const Point row_first =
                        (a.point - b.point) * 2.0L;
                    const Point row_second =
                        (a.point - c.point) * 2.0L;
                    const long double beta_first =
                        dot(a.point, a.point) - dot(b.point, b.point)
                        + b.weight * b.weight - a.weight * a.weight;
                    const long double beta_second =
                        dot(a.point, a.point) - dot(c.point, c.point)
                        + c.weight * c.weight - a.weight * a.weight;
                    const long double determinant =
                        cross(row_first, row_second);
                    if(plus_geometry_detail::sign(determinant) == 0){
                        Point axis = b.point - a.point;
                        if(plus_geometry_detail::close(a.point, b.point)){
                            axis = c.point - a.point;
                        }
                        const long double axis_length = std::sqrt(dot(axis, axis));
                        if(plus_geometry_detail::sign(axis_length) == 0) continue;
                        axis /= axis_length;
                        const long double first_coefficient = dot(row_first, axis);
                        const long double second_coefficient = dot(row_second, axis);
                        const long double first_rate = 2.0L * (b.weight - a.weight);
                        const long double second_rate = 2.0L * (c.weight - a.weight);
                        const long double scalar_determinant =
                            second_coefficient * first_rate
                            - first_coefficient * second_rate;
                        if(plus_geometry_detail::sign(scalar_determinant) == 0){
                            continue;
                        }
                        const long double along =
                            (first_rate * beta_second - second_rate * beta_first)
                            / scalar_determinant;
                        const long double radius =
                            (first_coefficient * beta_second
                                - second_coefficient * beta_first)
                            / scalar_determinant;
                        const long double a_coordinate = dot(a.point, axis);
                        const long double height_squared =
                            (radius + a.weight) * (radius + a.weight)
                            - (along - a_coordinate) * (along - a_coordinate);
                        if(plus_geometry_detail::sign(height_squared) < 0) continue;
                        const Point perpendicular{-axis.y, axis.x};
                        const Point base = axis * along
                            + (a.point - axis * a_coordinate);
                        const long double height =
                            std::sqrt(std::max(0.0L, height_squared));
                        register_vertex(
                            base + perpendicular * height, first, second, third
                        );
                        if(plus_geometry_detail::sign(height) != 0){
                            register_vertex(
                                base - perpendicular * height, first, second, third
                            );
                        }
                        continue;
                    }
                    const auto solve = [&](long double right_first,
                                           long double right_second){
                        return Point{
                            cross(
                                {right_first, row_first.y},
                                {right_second, row_second.y}
                            ) / determinant,
                            cross(
                                {row_first.x, right_first},
                                {row_second.x, right_second}
                            ) / determinant,
                        };
                    };
                    const Point base = solve(beta_first, beta_second);
                    const Point rate = solve(
                        2.0L * (b.weight - a.weight),
                        2.0L * (c.weight - a.weight)
                    );
                    const Point offset = base - a.point;
                    const long double qa = dot(rate, rate) - 1.0L;
                    const long double qb =
                        2.0L * (dot(offset, rate) - a.weight);
                    const long double qc =
                        dot(offset, offset) - a.weight * a.weight;
                    if(plus_geometry_detail::sign(qa) == 0){
                        if(plus_geometry_detail::sign(qb) != 0){
                            const long double radius = -qc / qb;
                            if(radius + a.weight >= 0.0L){
                                register_vertex(
                                    base + rate * radius, first, second, third
                                );
                            }
                        }
                        continue;
                    }
                    const long double discriminant =
                        qb * qb - 4.0L * qa * qc;
                    if(plus_geometry_detail::sign(discriminant) < 0) continue;
                    const long double root =
                        std::sqrt(std::max(0.0L, discriminant));
                    for(const long double radius: {
                        (-qb - root) / (2.0L * qa),
                        (-qb + root) / (2.0L * qa),
                    }){
                        if(radius + a.weight >= 0.0L){
                            register_vertex(
                                base + rate * radius, first, second, third
                            );
                        }
                    }
                }
            }
        }
    }

    bool pair_is_minimum(
        const ApolloniusCurve& curve,
        long double parameter
    ) const{
        const Point point = curve.point(parameter);
        const long double target =
            weighted_distance(curve.first_site, point);
        if(plus_geometry_detail::sign(
            target - weighted_distance(curve.second_site, point),
            std::max(1.0L, std::abs(target))
        ) != 0){
            return false;
        }
        for(std::size_t site = 0; site < sites_.size(); ++site){
            if(plus_geometry_detail::sign(
                weighted_distance(site, point) - target,
                std::max(1.0L, std::abs(target))
            ) < 0){
                return false;
            }
        }
        return true;
    }

    void compact_vertices(){
        const std::size_t no_vertex = ApolloniusEdge::no_vertex;
        std::vector<std::size_t> remap(vertices_.size(), no_vertex);
        std::vector<std::size_t> used;
        for(const ApolloniusEdge& edge: edges_){
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
        for(ApolloniusEdge& edge: edges_){
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
        for(std::size_t curve_index = 0;
            curve_index < curves_.size();
            ++curve_index){
            const ApolloniusCurve& curve = curves_[curve_index];
            std::vector<std::pair<long double, std::size_t>> events =
                curve_events_[curve_index];
            std::sort(events.begin(), events.end());
            events.erase(std::unique(
                events.begin(), events.end(),
                [](const auto& left, const auto& right){
                    return plus_geometry_detail::sign(
                        left.first - right.first,
                        std::max({
                            1.0L, std::abs(left.first), std::abs(right.first)
                        })
                    ) == 0;
                }
            ), events.end());
            const long double lower =
                curve.kind == ApolloniusCurveKind::ray
                ? 0.0L
                : -std::numeric_limits<long double>::infinity();
            std::vector<long double> cuts{lower};
            for(const auto& event: events) cuts.push_back(event.first);
            cuts.push_back(std::numeric_limits<long double>::infinity());
            for(std::size_t interval = 1;
                interval < cuts.size();
                ++interval){
                long double sample = 0.0L;
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
                if(!pair_is_minimum(curve, sample)) continue;
                const std::size_t edge = edges_.size();
                edges_.push_back({
                    curve_index,
                    interval >= 2
                        ? events[interval - 2].second
                        : ApolloniusEdge::no_vertex,
                    interval - 1 < events.size()
                        ? events[interval - 1].second
                        : ApolloniusEdge::no_vertex,
                    cuts[interval - 1],
                    cuts[interval],
                });
                cell_edges_[curve.first_site].push_back(edge);
                cell_edges_[curve.second_site].push_back(edge);
            }
        }
    }

    std::vector<WeightedPoint> sites_;
    std::vector<ApolloniusCurve> curves_;
    std::vector<Point> vertices_;
    std::vector<ApolloniusEdge> edges_;
    std::vector<std::vector<std::size_t>> cell_edges_;
    std::vector<std::size_t> curve_index_;
    std::vector<
        std::vector<std::pair<long double, std::size_t>>
    > curve_events_;
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TYPE_DEFINITION_APOLLONIUS_DIAGRAM_HPP_INCLUDED
