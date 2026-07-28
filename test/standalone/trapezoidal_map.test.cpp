// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/2d/detail/numeric/plus_geometry_detail.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point_point/dot.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/trapezoidal_map.hpp"

namespace{

std::uint64_t next_random(std::uint64_t& state){
    state ^= state << 7;
    state ^= state >> 9;
    return state;
}

bool point_on_segment(const Segment& segment, const Point& point){
    return plus_geometry_detail::orientation(segment.a, segment.b, point) == 0
        && plus_geometry_detail::sign(
            dot(point - segment.a, point - segment.b),
            plus_geometry_detail::squared_distance(segment.a, segment.b)
        ) <= 0;
}

long double y_at(const Segment& segment, long double x){
    return segment.a.y + (segment.b.y - segment.a.y)
        * (x - segment.a.x) / (segment.b.x - segment.a.x);
}

void verify_off_event(
    const TrapezoidalMap& map,
    const std::vector<Segment>& segments,
    const Point& point
){
    std::size_t expected_below = TrapezoidalMapLocation::no_segment;
    std::size_t expected_above = TrapezoidalMapLocation::no_segment;
    long double below_y = -std::numeric_limits<long double>::infinity();
    long double above_y = std::numeric_limits<long double>::infinity();
    for(std::size_t index = 0; index < segments.size(); ++index){
        const Segment& segment = segments[index];
        const long double minimum_x = std::min(segment.a.x, segment.b.x);
        const long double maximum_x = std::max(segment.a.x, segment.b.x);
        if(!(minimum_x < point.x && point.x < maximum_x)) continue;
        const long double y = y_at(segment, point.x);
        if(y < point.y && y > below_y){
            below_y = y;
            expected_below = index;
        }
        if(y > point.y && y < above_y){
            above_y = y;
            expected_above = index;
        }
    }
    const TrapezoidalMapLocation actual = map.locate(point);
    assert(actual.on_segment == TrapezoidalMapLocation::no_segment);
    assert(actual.below == expected_below);
    assert(actual.above == expected_above);
}

void run_case(std::uint64_t seed){
    constexpr int layer_count = 30;
    constexpr int piece_count = 60;
    std::vector<Segment> segments;
    segments.reserve(layer_count * piece_count + 40);
    for(int layer = 0; layer < layer_count; ++layer){
        const auto ordinate = [layer](int x){
            return static_cast<long double>(layer * 100
                + ((x * x + layer * 13) % 31) - 15);
        };
        for(int piece = 0; piece < piece_count; ++piece){
            segments.push_back({
                {static_cast<long double>(piece), ordinate(piece)},
                {static_cast<long double>(piece + 1), ordinate(piece + 1)},
            });
        }
    }
    const std::size_t vertical_begin = segments.size();
    for(int index = 0; index < 40; ++index){
        segments.push_back({
            {1000, static_cast<long double>(index * 10)},
            {1000, static_cast<long double>(index * 10 + 5)},
        });
    }

    TrapezoidalMap map(segments);
    assert(map.segment_count() == segments.size());
    assert(map.slab_count() == 63);

    for(int query_index = 0; query_index < 5000; ++query_index){
        const int piece = static_cast<int>(next_random(seed) % piece_count);
        const Point query{
            static_cast<long double>(piece) + 0.375L,
            static_cast<long double>(
                static_cast<int>(next_random(seed) % 330001) - 15001
            ) / 100.0L + 0.0001L,
        };
        verify_off_event(map, segments, query);

        const int layer = static_cast<int>(next_random(seed) % layer_count);
        const std::size_t index = static_cast<std::size_t>(
            layer * piece_count + piece
        );
        const Point midpoint = (segments[index].a + segments[index].b) / 2.0L;
        const auto on = map.locate(midpoint);
        assert(on.on_segment != TrapezoidalMapLocation::no_segment);
        assert(point_on_segment(segments[on.on_segment], midpoint));
    }

    for(int layer = 0; layer < layer_count; ++layer){
        for(int event = 0; event <= piece_count; event += 3){
            const long double y = static_cast<long double>(layer * 100
                + ((event * event + layer * 13) % 31) - 15);
            const Point endpoint{static_cast<long double>(event), y};
            const auto located = map.locate(endpoint);
            assert(located.on_segment != TrapezoidalMapLocation::no_segment);
            assert(point_on_segment(segments[located.on_segment], endpoint));
        }
    }

    for(std::size_t index = vertical_begin; index < segments.size(); ++index){
        const Point midpoint = (segments[index].a + segments[index].b) / 2.0L;
        const auto located = map.locate(midpoint);
        assert(located.on_segment != TrapezoidalMapLocation::no_segment);
        assert(point_on_segment(segments[located.on_segment], midpoint));
    }

    bool rejected = false;
    try{
        static_cast<void>(TrapezoidalMap({{{0, 0}, {0, 0}}}));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
}

}  // namespace

int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    for(int case_index = 0; case_index < cases; ++case_index){
        run_case(0x9e3779b97f4a7c15ULL
            ^ static_cast<std::uint64_t>(case_index + 1));
        std::cout << "OK\n";
    }
}
