#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "advanced/detail.hpp"
#include "segment_intersection_kind.hpp"

struct SegmentIntersectionRecord{
    std::size_t first;
    std::size_t second;
    SegmentIntersectionKind kind;
    Point first_point;
    Point second_point;
};

namespace bentley_ottmann_detail{

struct SweepSegment{
    Segment original;
    Point left;
    Point right;
    long double slope;
    bool point;
};

inline bool point_less_exact(const Point& first, const Point& second){
    if(first.x != second.x) return first.x < second.x;
    return first.y < second.y;
}

inline long double choose_shear(const std::vector<Segment>& segments){
    if(segments.size()
        > (std::numeric_limits<std::size_t>::max() - 9) / 6){
        throw std::length_error("too many segments for sweep shear selection");
    }
    const std::size_t candidate_count = 6 * segments.size() + 9;
    const long double maximum_shear =
        static_cast<long double>(candidate_count - 1);
    const long double safe_coordinate_bound = std::sqrt(
        std::numeric_limits<long double>::max()
    ) / (32.0L * (maximum_shear + 1.0L));
    for(const Segment& segment: segments){
        for(const Point& point: {segment.a, segment.b}){
            if(!std::isfinite(point.x) || !std::isfinite(point.y)){
                throw std::invalid_argument(
                    "sweep segment coordinates must be finite"
                );
            }
            if(std::abs(point.x) > safe_coordinate_bound
                || std::abs(point.y) > safe_coordinate_bound){
                throw std::overflow_error(
                    "sweep shear arithmetic would overflow"
                );
            }
        }
    }

    std::vector<std::ptrdiff_t> forbidden_difference(
        candidate_count + 1, 0
    );
    for(const Segment& segment: segments){
        if(advanced_geometry_detail::point_equal(segment.a, segment.b)){
            continue;
        }
        const long double difference_x = segment.b.x - segment.a.x;
        const long double difference_y = segment.b.y - segment.a.y;
        if(difference_y == 0.0L) continue;
        const long double center = -difference_x / difference_y;
        if(!std::isfinite(center)) continue;
        const long double radius = GEOMETRY_EPS * std::max(
            1.0L / std::abs(difference_y),
            std::abs(center) + maximum_shear
        );
        const long double lower_value =
            std::floor(center - radius) - 1.0L;
        const long double upper_value =
            std::ceil(center + radius) + 1.0L;
        if(upper_value < 0.0L || lower_value > maximum_shear) continue;
        const std::size_t lower = lower_value <= 0.0L
            ? 0
            : static_cast<std::size_t>(lower_value);
        const std::size_t upper = upper_value >= maximum_shear
            ? candidate_count - 1
            : static_cast<std::size_t>(upper_value);
        ++forbidden_difference[lower];
        --forbidden_difference[upper + 1];
    }
    std::ptrdiff_t forbidden_count = 0;
    for(std::size_t candidate = 0; candidate < candidate_count; ++candidate){
        forbidden_count += forbidden_difference[candidate];
        if(forbidden_count == 0){
            return static_cast<long double>(candidate);
        }
    }
    throw std::invalid_argument(
        "failed to choose a numerically nonvertical sweep direction"
    );
}

inline Point transform(const Point& point, long double shear){
    return {point.x + shear * point.y, point.y};
}

struct SweepContext{
    const std::vector<SweepSegment>* segments = nullptr;
    long double x = 0.0L;
    int phase = -1;
    long double event_y = 0.0L;
    std::size_t probe_id = 0;
    long double probe_y = 0.0L;
    long double probe_slope = 0.0L;
};

inline std::pair<long double, long double> status_key(
    const SweepContext& context,
    std::size_t index
){
    if(index == context.probe_id){
        return {context.probe_y, context.probe_slope};
    }
    const SweepSegment& segment = (*context.segments)[index];
    const long double ratio =
        (context.x - segment.left.x) / (segment.right.x - segment.left.x);
    long double y = segment.left.y
        + (segment.right.y - segment.left.y) * ratio;
    if(advanced_geometry_detail::scaled_sign(
        y - context.event_y, std::abs(y) + std::abs(context.event_y)
    ) == 0){
        y = context.event_y;
    }
    return {y, segment.slope};
}

struct StatusComparator{
    SweepContext* context;

    bool operator()(std::size_t first, std::size_t second) const{
        if(first == second) return false;
        const auto first_key = status_key(*context, first);
        const auto second_key = status_key(*context, second);
        if(first_key.first != second_key.first){
            return first_key.first < second_key.first;
        }
        const long double first_slope =
            static_cast<long double>(context->phase) * first_key.second;
        const long double second_slope =
            static_cast<long double>(context->phase) * second_key.second;
        if(first_slope != second_slope) return first_slope < second_slope;
        return first < second;
    }
};

enum EventType{
    START_EVENT,
    END_EVENT,
    POINT_SEGMENT_EVENT,
    INTERSECTION_EVENT,
};

struct Event{
    Point point;
    Point sweep_point;
    EventType type;
    std::size_t first;
    std::size_t second;
};

struct EventGreater{
    bool operator()(const Event& first, const Event& second) const{
        if(first.sweep_point.x != second.sweep_point.x){
            return first.sweep_point.x > second.sweep_point.x;
        }
        if(first.sweep_point.y != second.sweep_point.y){
            return first.sweep_point.y > second.sweep_point.y;
        }
        if(first.type != second.type) return first.type > second.type;
        if(first.first != second.first) return first.first > second.first;
        return first.second > second.second;
    }
};

inline std::pair<std::size_t, std::size_t> ordered_pair(
    std::size_t first,
    std::size_t second
){
    if(second < first) std::swap(first, second);
    return {first, second};
}

inline bool event_is_after(const Point& point, const Point& current){
    if(advanced_geometry_detail::point_equal(point, current)) return false;
    return point_less_exact(current, point);
}

inline std::vector<SegmentIntersectionRecord> intersections(
    const std::vector<Segment>& segments,
    std::size_t maximum_results
){
    using namespace advanced_geometry_detail;
    if(maximum_results == 0 || segments.size() < 2) return {};

    const long double shear = choose_shear(segments);
    const std::size_t count = segments.size();
    std::vector<SweepSegment> sweep_segments(count);
    std::priority_queue<Event, std::vector<Event>, EventGreater> events;
    for(std::size_t index = 0; index < count; ++index){
        const Point first = transform(segments[index].a, shear);
        const Point second = transform(segments[index].b, shear);
        if(point_equal(first, second)){
            sweep_segments[index] = {
                segments[index], first, second, 0.0L, true
            };
            events.push({segments[index].a, first, POINT_SEGMENT_EVENT, index, index});
            continue;
        }
        const bool forward = point_less_exact(first, second);
        const Point left = forward ? first : second;
        const Point right = forward ? second : first;
        const Point left_original = forward ? segments[index].a : segments[index].b;
        const Point right_original = forward ? segments[index].b : segments[index].a;
        sweep_segments[index] = {
            segments[index], left, right,
            (right.y - left.y) / (right.x - left.x), false
        };
        events.push({left_original, left, START_EVENT, index, index});
        events.push({right_original, right, END_EVENT, index, index});
    }

    SweepContext context{
        &sweep_segments, 0.0L, -1, 0.0L, count, 0.0L, 0.0L
    };
    using Status = std::set<std::size_t, StatusComparator>;
    Status status(StatusComparator{&context});
    std::set<std::pair<std::size_t, std::size_t>> scheduled;
    std::set<std::pair<std::size_t, std::size_t>> reported;
    std::vector<SegmentIntersectionRecord> result;

    const auto record_pair = [&](std::size_t first, std::size_t second){
        const auto pair = ordered_pair(first, second);
        if(pair.first == pair.second || reported.contains(pair)) return false;
        const std::vector<Point> geometry = segment_intersection_set(
            segments[pair.first], segments[pair.second]
        );
        if(geometry.empty()) return false;
        reported.insert(pair);
        if(geometry.size() == 1){
            result.push_back({
                pair.first, pair.second, SEGMENT_INTERSECTION_POINT,
                geometry[0], geometry[0]
            });
        }else{
            result.push_back({
                pair.first, pair.second, SEGMENT_INTERSECTION_SEGMENT,
                geometry[0], geometry[1]
            });
        }
        return result.size() >= maximum_results;
    };

    const auto containing_segments = [&](const Point& point){
        std::vector<std::size_t> found;
        if(status.empty()) return found;
        context.probe_y = point.y;
        context.probe_slope = context.phase > 0
            ? -std::numeric_limits<long double>::infinity()
            : std::numeric_limits<long double>::infinity();
        auto iterator = status.lower_bound(context.probe_id);
        while(iterator != status.begin()){
            const auto previous = std::prev(iterator);
            if(!on_segment_scaled(segments[*previous], point)) break;
            iterator = previous;
        }
        while(iterator != status.end()
            && on_segment_scaled(segments[*iterator], point)){
            found.push_back(*iterator);
            ++iterator;
        }
        return found;
    };

    const auto schedule_pair = [&](std::optional<std::size_t> first,
                                   std::optional<std::size_t> second,
                                   const Point& current_point,
                                   const Point& current_sweep_point){
        if(!first.has_value() || !second.has_value()
            || *first == *second){
            return false;
        }
        const auto pair = ordered_pair(*first, *second);
        if(reported.contains(pair) || scheduled.contains(pair)) return false;
        const std::vector<Point> geometry = segment_intersection_set(
            segments[pair.first], segments[pair.second]
        );
        if(geometry.empty()) return false;
        if(geometry.size() == 2){
            return record_pair(pair.first, pair.second);
        }
        const Point sweep_point = transform(geometry[0], shear);
        if(point_equal(sweep_point, current_sweep_point)){
            return record_pair(pair.first, pair.second);
        }
        if(!event_is_after(sweep_point, current_sweep_point)) return false;
        scheduled.insert(pair);
        events.push({
            geometry[0], sweep_point, INTERSECTION_EVENT,
            pair.first, pair.second
        });
        (void)current_point;
        return false;
    };

    while(!events.empty()){
        Event first_event = events.top();
        events.pop();
        std::vector<Event> group{first_event};
        while(!events.empty() && point_equal(
            events.top().sweep_point, first_event.sweep_point
        )){
            group.push_back(events.top());
            events.pop();
        }

        const Point point = first_event.point;
        const Point sweep_point = first_event.sweep_point;
        context.x = sweep_point.x;
        context.phase = -1;
        context.event_y = sweep_point.y;

        std::vector<std::size_t> starts, ends, point_segments, event_segments;
        for(const Event& event: group){
            if(event.type == START_EVENT) starts.push_back(event.first);
            else if(event.type == END_EVENT) ends.push_back(event.first);
            else if(event.type == POINT_SEGMENT_EVENT){
                point_segments.push_back(event.first);
            }else{
                event_segments.push_back(event.first);
                event_segments.push_back(event.second);
            }
        }
        const auto make_unique = [](std::vector<std::size_t>& values){
            std::sort(values.begin(), values.end());
            values.erase(std::unique(values.begin(), values.end()), values.end());
        };
        make_unique(starts);
        make_unique(ends);
        make_unique(point_segments);
        make_unique(event_segments);

        std::vector<std::size_t> before = containing_segments(point);
        std::optional<std::size_t> gap_lower, gap_upper;
        if(!before.empty()){
            auto first_iterator = status.find(before.front());
            if(first_iterator != status.begin()){
                gap_lower = *std::prev(first_iterator);
            }
            auto last_iterator = status.find(before.back());
            if(last_iterator != status.end()){
                ++last_iterator;
                if(last_iterator != status.end()) gap_upper = *last_iterator;
            }
        }

        std::vector<bool> ending(count, false);
        for(const std::size_t index: ends) ending[index] = true;
        for(const std::size_t index: before){
            status.erase(index);
        }

        std::vector<std::size_t> inserted;
        for(const std::size_t index: before){
            if(!ending[index]) inserted.push_back(index);
        }
        inserted.insert(inserted.end(), starts.begin(), starts.end());
        make_unique(inserted);

        context.phase = 1;
        for(const std::size_t index: inserted){
            if(sweep_segments[index].point) continue;
            status.insert(index);
        }

        std::vector<std::size_t> after = containing_segments(point);
        std::vector<std::size_t> meeting = before;
        meeting.insert(meeting.end(), after.begin(), after.end());
        meeting.insert(meeting.end(), starts.begin(), starts.end());
        meeting.insert(meeting.end(), ends.begin(), ends.end());
        meeting.insert(
            meeting.end(), point_segments.begin(), point_segments.end()
        );
        meeting.insert(
            meeting.end(), event_segments.begin(), event_segments.end()
        );
        make_unique(meeting);
        for(std::size_t first = 0; first < meeting.size(); ++first){
            for(std::size_t second = first + 1; second < meeting.size(); ++second){
                if(record_pair(meeting[first], meeting[second])){
                    std::sort(result.begin(), result.end(), [](const auto& left, const auto& right){
                        return std::pair{left.first, left.second}
                            < std::pair{right.first, right.second};
                    });
                    return result;
                }
            }
        }

        if(!inserted.empty()){
            std::sort(inserted.begin(), inserted.end(), status.key_comp());
            auto lower_iterator = status.find(inserted.front());
            auto upper_iterator = status.find(inserted.back());
            std::optional<std::size_t> lower_neighbor, upper_neighbor;
            if(lower_iterator != status.begin()){
                lower_neighbor = *std::prev(lower_iterator);
            }
            if(upper_iterator != status.end()){
                ++upper_iterator;
                if(upper_iterator != status.end()) upper_neighbor = *upper_iterator;
            }
            if(schedule_pair(
                lower_neighbor, inserted.front(), point, sweep_point
            ) || schedule_pair(
                inserted.back(), upper_neighbor, point, sweep_point
            )){
                std::sort(result.begin(), result.end(), [](const auto& left, const auto& right){
                    return std::pair{left.first, left.second}
                        < std::pair{right.first, right.second};
                });
                return result;
            }
        }else if(schedule_pair(gap_lower, gap_upper, point, sweep_point)){
            std::sort(result.begin(), result.end(), [](const auto& left, const auto& right){
                return std::pair{left.first, left.second}
                    < std::pair{right.first, right.second};
            });
            return result;
        }
    }

    std::sort(result.begin(), result.end(), [](const auto& left, const auto& right){
        return std::pair{left.first, left.second}
            < std::pair{right.first, right.second};
    });
    return result;
}

}  // namespace bentley_ottmann_detail

inline std::vector<SegmentIntersectionRecord> bentley_ottmann_intersections(
    const std::vector<Segment>& segments
){
    return bentley_ottmann_detail::intersections(
        segments, std::numeric_limits<std::size_t>::max()
    );
}
