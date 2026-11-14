// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/type/definition/delaunay_triangulation_result.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set/delaunay_triangulation_points.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/delaunay_triangulation.hpp"

namespace{

using IntegerPoint = std::pair<long long, long long>;

__int128 cross(
    const IntegerPoint& first,
    const IntegerPoint& second,
    const IntegerPoint& third
){
    const __int128 first_x = static_cast<__int128>(second.first) - first.first;
    const __int128 first_y = static_cast<__int128>(second.second) - first.second;
    const __int128 second_x = static_cast<__int128>(third.first) - first.first;
    const __int128 second_y = static_cast<__int128>(third.second) - first.second;
    return first_x * second_y - first_y * second_x;
}

bool in_circumcircle_exact(
    const IntegerPoint& first,
    const IntegerPoint& second,
    const IntegerPoint& third,
    const IntegerPoint& query
){
    const __int128 first_x = static_cast<__int128>(first.first) - query.first;
    const __int128 first_y = static_cast<__int128>(first.second) - query.second;
    const __int128 second_x = static_cast<__int128>(second.first) - query.first;
    const __int128 second_y = static_cast<__int128>(second.second) - query.second;
    const __int128 third_x = static_cast<__int128>(third.first) - query.first;
    const __int128 third_y = static_cast<__int128>(third.second) - query.second;
    const __int128 first_norm = first_x * first_x + first_y * first_y;
    const __int128 second_norm = second_x * second_x + second_y * second_y;
    const __int128 third_norm = third_x * third_x + third_y * third_y;
    const __int128 determinant =
        (second_x * third_y - second_y * third_x) * first_norm
        + (third_x * first_y - third_y * first_x) * second_norm
        + (first_x * second_y - first_y * second_x) * third_norm;
    return determinant > 0;
}

bool proper_intersection(
    const IntegerPoint& first,
    const IntegerPoint& second,
    const IntegerPoint& third,
    const IntegerPoint& fourth
){
    const __int128 first_side = cross(first, second, third);
    const __int128 second_side = cross(first, second, fourth);
    const __int128 third_side = cross(third, fourth, first);
    const __int128 fourth_side = cross(third, fourth, second);
    return ((first_side < 0 && second_side > 0) || (first_side > 0 && second_side < 0))
        && ((third_side < 0 && fourth_side > 0)
            || (third_side > 0 && fourth_side < 0));
}

std::size_t hull_boundary_size(std::vector<IntegerPoint> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    if(points.size() <= 2) return points.size();
    bool collinear = true;
    for(std::size_t index = 2; index < points.size(); ++index){
        if(cross(points[0], points[1], points[index]) != 0) collinear = false;
    }
    if(collinear) return points.size();

    std::vector<IntegerPoint> hull;
    for(const IntegerPoint& point: points){
        while(hull.size() >= 2
            && cross(hull[hull.size() - 2], hull.back(), point) < 0){
            hull.pop_back();
        }
        hull.push_back(point);
    }
    const std::size_t lower = hull.size();
    for(std::size_t index = points.size() - 1; index-- > 0;){
        const IntegerPoint& point = points[index];
        while(hull.size() > lower
            && cross(hull[hull.size() - 2], hull.back(), point) < 0){
            hull.pop_back();
        }
        hull.push_back(point);
    }
    hull.pop_back();
    return hull.size();
}

void verify(const std::vector<IntegerPoint>& integer_points){
    std::vector<Point> points;
    points.reserve(integer_points.size());
    for(const auto& [x, y]: integer_points){
        points.emplace_back(static_cast<long double>(x), static_cast<long double>(y));
    }
    const DelaunayTriangulationResult result = delaunay_triangulation(points);
    assert(result.representative.size() == points.size());

    std::map<IntegerPoint, std::size_t> first_occurrence;
    for(std::size_t index = 0; index < integer_points.size(); ++index){
        first_occurrence.try_emplace(integer_points[index], index);
        assert(result.representative[index] == first_occurrence[integer_points[index]]);
    }
    assert(result.sites.size() == first_occurrence.size());
    for(std::size_t index = 1; index < result.sites.size(); ++index){
        assert(integer_points[result.sites[index - 1]]
            < integer_points[result.sites[index]]);
    }

    assert(std::is_sorted(result.edges.begin(), result.edges.end()));
    assert(std::adjacent_find(result.edges.begin(), result.edges.end())
        == result.edges.end());
    std::set<std::pair<std::size_t, std::size_t>> edge_set;
    for(const auto& edge: result.edges){
        assert(edge.first < edge.second);
        assert(edge.second < points.size());
        assert(result.representative[edge.first] == edge.first);
        assert(result.representative[edge.second] == edge.second);
        edge_set.insert(edge);
    }

    assert(std::is_sorted(result.triangles.begin(), result.triangles.end()));
    for(const auto& triangle: result.triangles){
        assert(triangle[0] == std::min({triangle[0], triangle[1], triangle[2]}));
        assert(triangle[0] < points.size()
            && triangle[1] < points.size()
            && triangle[2] < points.size());
        assert(cross(
            integer_points[triangle[0]],
            integer_points[triangle[1]],
            integer_points[triangle[2]]
        ) > 0);
        for(int edge = 0; edge < 3; ++edge){
            std::size_t first = triangle[static_cast<std::size_t>(edge)];
            std::size_t second = triangle[static_cast<std::size_t>((edge + 1) % 3)];
            if(first > second) std::swap(first, second);
            assert(edge_set.contains({first, second}));
        }
        const std::size_t sample_count = result.sites.size() <= 120
            ? result.sites.size()
            : 32;
        for(std::size_t sample = 0; sample < sample_count; ++sample){
            const std::size_t site_index = result.sites.size() <= 120
                ? sample
                : (
                    triangle[0] * std::size_t{1'000'003}
                    + triangle[1] * std::size_t{97'409}
                    + sample * std::size_t{65'537}
                ) % result.sites.size();
            const std::size_t site = result.sites[site_index];
            assert(!in_circumcircle_exact(
                integer_points[triangle[0]],
                integer_points[triangle[1]],
                integer_points[triangle[2]],
                integer_points[site]
            ));
        }
    }

    if(result.sites.size() <= 120){
        for(std::size_t first = 0; first < result.edges.size(); ++first){
            for(std::size_t second = first + 1; second < result.edges.size(); ++second){
                const auto& left = result.edges[first];
                const auto& right = result.edges[second];
                if(left.first == right.first || left.first == right.second
                    || left.second == right.first || left.second == right.second){
                    continue;
                }
                assert(!proper_intersection(
                    integer_points[left.first],
                    integer_points[left.second],
                    integer_points[right.first],
                    integer_points[right.second]
                ));
            }
        }
    }

    const std::size_t unique_count = result.sites.size();
    if(unique_count <= 1){
        assert(result.edges.empty() && result.triangles.empty());
        return;
    }
    std::vector<IntegerPoint> unique_points;
    for(std::size_t site: result.sites) unique_points.push_back(integer_points[site]);
    const std::size_t boundary = hull_boundary_size(unique_points);
    bool collinear = true;
    for(std::size_t index = 2; index < unique_points.size(); ++index){
        if(cross(unique_points[0], unique_points[1], unique_points[index]) != 0){
            collinear = false;
        }
    }
    if(collinear){
        assert(result.edges.size() == unique_count - 1);
        assert(result.triangles.empty());
    }else{
        assert(result.edges.size() == 3 * unique_count - 3 - boundary);
        assert(result.triangles.size() == 2 * unique_count - 2 - boundary);
    }
}

// 浮動小数点座標の厳密な参照実装。各座標を共通の 2 の冪で割った整数として多倍長で扱う。
struct BigInteger{
    bool negative = false;
    std::vector<std::uint32_t> limbs;

    void trim(){
        while(!limbs.empty() && limbs.back() == 0) limbs.pop_back();
        if(limbs.empty()) negative = false;
    }
    int sign() const{ return limbs.empty() ? 0 : (negative ? -1 : 1); }
};

int compare_magnitude(const BigInteger& first, const BigInteger& second){
    if(first.limbs.size() != second.limbs.size()){
        return first.limbs.size() < second.limbs.size() ? -1 : 1;
    }
    for(std::size_t index = first.limbs.size(); index-- > 0;){
        if(first.limbs[index] != second.limbs[index]){
            return first.limbs[index] < second.limbs[index] ? -1 : 1;
        }
    }
    return 0;
}

BigInteger add_magnitude(const BigInteger& first, const BigInteger& second){
    BigInteger result;
    std::uint64_t carry = 0;
    for(std::size_t index = 0;
        index < std::max(first.limbs.size(), second.limbs.size()) || carry != 0;
        ++index){
        std::uint64_t sum = carry;
        if(index < first.limbs.size()) sum += first.limbs[index];
        if(index < second.limbs.size()) sum += second.limbs[index];
        result.limbs.push_back(static_cast<std::uint32_t>(sum));
        carry = sum >> 32;
    }
    result.trim();
    return result;
}

BigInteger subtract_magnitude(const BigInteger& first, const BigInteger& second){
    BigInteger result;
    std::int64_t borrow = 0;
    for(std::size_t index = 0; index < first.limbs.size(); ++index){
        std::int64_t difference = static_cast<std::int64_t>(first.limbs[index]) - borrow
            - (index < second.limbs.size()
                ? static_cast<std::int64_t>(second.limbs[index]) : 0);
        borrow = difference < 0 ? 1 : 0;
        if(difference < 0) difference += std::int64_t{1} << 32;
        result.limbs.push_back(static_cast<std::uint32_t>(difference));
    }
    result.trim();
    return result;
}

BigInteger operator+(const BigInteger& first, const BigInteger& second){
    BigInteger result;
    if(first.negative == second.negative){
        result = add_magnitude(first, second);
        result.negative = first.negative;
    }else if(compare_magnitude(first, second) >= 0){
        result = subtract_magnitude(first, second);
        result.negative = first.negative;
    }else{
        result = subtract_magnitude(second, first);
        result.negative = second.negative;
    }
    result.trim();
    return result;
}

BigInteger operator-(const BigInteger& first, BigInteger second){
    second.negative = !second.negative;
    second.trim();
    return first + second;
}

BigInteger operator*(const BigInteger& first, const BigInteger& second){
    BigInteger result;
    result.limbs.assign(first.limbs.size() + second.limbs.size() + 1, 0);
    for(std::size_t left = 0; left < first.limbs.size(); ++left){
        std::uint64_t carry = 0;
        for(std::size_t right = 0; right < second.limbs.size() || carry != 0; ++right){
            const std::uint64_t current = result.limbs[left + right] + carry
                + (right < second.limbs.size()
                    ? static_cast<std::uint64_t>(first.limbs[left]) * second.limbs[right]
                    : 0);
            result.limbs[left + right] = static_cast<std::uint32_t>(current);
            carry = current >> 32;
        }
    }
    result.negative = first.negative != second.negative;
    result.trim();
    return result;
}

struct BigPoint{
    BigInteger x;
    BigInteger y;
};

std::vector<BigPoint> to_big_points(const std::vector<Point>& points){
    const auto lowest_exponent = [](long double value){
        int exponent = 0;
        static_cast<void>(std::frexp(value, &exponent));
        return exponent - 64;
    };
    int base = std::numeric_limits<int>::max();
    for(const Point& point: points){
        if(point.x != 0.0L) base = std::min(base, lowest_exponent(point.x));
        if(point.y != 0.0L) base = std::min(base, lowest_exponent(point.y));
    }
    const auto convert = [&](long double value){
        BigInteger result;
        if(value == 0.0L) return result;
        int exponent = 0;
        const long double fraction = std::frexp(std::abs(value), &exponent);
        const auto mantissa = static_cast<std::uint64_t>(std::ldexp(fraction, 64));
        result.limbs = {
            static_cast<std::uint32_t>(mantissa),
            static_cast<std::uint32_t>(mantissa >> 32),
        };
        const int shift = exponent - 64 - base;
        BigInteger power;
        power.limbs.assign(static_cast<std::size_t>(shift / 32) + 1, 0);
        power.limbs.back() = std::uint32_t{1} << (shift % 32);
        result = result * power;
        result.negative = value < 0.0L;
        return result;
    };
    std::vector<BigPoint> result;
    for(const Point& point: points) result.push_back({convert(point.x), convert(point.y)});
    return result;
}

int big_orientation(const BigPoint& first, const BigPoint& second, const BigPoint& third){
    return ((second.x - first.x) * (third.y - first.y)
        - (second.y - first.y) * (third.x - first.x)).sign();
}

int big_circumcircle_sign(
    const BigPoint& first,
    const BigPoint& second,
    const BigPoint& third,
    const BigPoint& query
){
    const BigInteger first_x = first.x - query.x;
    const BigInteger first_y = first.y - query.y;
    const BigInteger second_x = second.x - query.x;
    const BigInteger second_y = second.y - query.y;
    const BigInteger third_x = third.x - query.x;
    const BigInteger third_y = third.y - query.y;
    const BigInteger first_norm = first_x * first_x + first_y * first_y;
    const BigInteger second_norm = second_x * second_x + second_y * second_y;
    const BigInteger third_norm = third_x * third_x + third_y * third_y;
    return ((second_x * third_y - second_y * third_x) * first_norm
        + (third_x * first_y - third_y * first_x) * second_norm
        + (first_x * second_y - first_y * second_x) * third_norm).sign();
}

// 浮動小数点座標の入力に対し、出力が入力座標そのものの厳密な Delaunay 三角形分割であることを
// 総当たりで確かめる。
void verify_floating(const std::vector<Point>& points){
    const DelaunayTriangulationResult result = delaunay_triangulation(points);
    const std::vector<BigPoint> exact = to_big_points(points);
    const std::set<std::pair<std::size_t, std::size_t>> edge_set(
        result.edges.begin(), result.edges.end()
    );
    for(const auto& triangle: result.triangles){
        assert(big_orientation(exact[triangle[0]], exact[triangle[1]], exact[triangle[2]]) > 0);
        for(int edge = 0; edge < 3; ++edge){
            std::size_t first = triangle[static_cast<std::size_t>(edge)];
            std::size_t second = triangle[static_cast<std::size_t>((edge + 1) % 3)];
            if(first > second) std::swap(first, second);
            assert(edge_set.contains({first, second}));
        }
        for(std::size_t site: result.sites){
            assert(big_circumcircle_sign(
                exact[triangle[0]], exact[triangle[1]], exact[triangle[2]], exact[site]
            ) <= 0);
        }
    }
    for(const auto& [first, second]: result.edges){
        if(result.edges.size() > 40) break;
        for(const auto& [third, fourth]: result.edges){
            if(first == third || first == fourth || second == third || second == fourth){
                continue;
            }
            const int first_side = big_orientation(exact[first], exact[second], exact[third]);
            const int second_side = big_orientation(exact[first], exact[second], exact[fourth]);
            const int third_side = big_orientation(exact[third], exact[fourth], exact[first]);
            const int fourth_side = big_orientation(exact[third], exact[fourth], exact[second]);
            assert(!(first_side * second_side < 0 && third_side * fourth_side < 0));
        }
    }

    std::vector<std::size_t> sites = result.sites;
    const std::size_t site_count = sites.size();
    if(site_count < 2){
        assert(result.edges.empty() && result.triangles.empty());
        return;
    }
    bool collinear = true;
    for(std::size_t index = 2; index < site_count; ++index){
        if(big_orientation(exact[sites[0]], exact[sites[1]], exact[sites[index]]) != 0){
            collinear = false;
        }
    }
    if(collinear){
        assert(result.triangles.empty());
        assert(result.edges.size() == site_count - 1);
        return;
    }
    std::sort(sites.begin(), sites.end(), [&](std::size_t first, std::size_t second){
        if(points[first].x != points[second].x) return points[first].x < points[second].x;
        return points[first].y < points[second].y;
    });
    std::vector<std::size_t> hull;
    for(int pass = 0; pass < 2; ++pass){
        const std::size_t base = hull.size();
        for(std::size_t site: sites){
            while(hull.size() >= base + 2 && big_orientation(
                exact[hull[hull.size() - 2]], exact[hull.back()], exact[site]
            ) < 0){
                hull.pop_back();
            }
            hull.push_back(site);
        }
        hull.pop_back();
        std::reverse(sites.begin(), sites.end());
    }
    const std::size_t boundary = hull.size();
    assert(result.edges.size() == 3 * site_count - 3 - boundary);
    assert(result.triangles.size() == 2 * site_count - 2 - boundary);
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const DelaunayTriangulationResult default_result;
    assert(default_result.triangles.empty());
    assert(default_result.edges.empty());
    assert(default_result.representative.empty());
    assert(default_result.sites.empty());

    bool invalid_thrown = false;
    try{
        static_cast<void>(delaunay_triangulation({
            {std::numeric_limits<long double>::infinity(), 0.0L}
        }));
    }catch(const std::invalid_argument&){
        invalid_thrown = true;
    }
    assert(invalid_thrown);

    bool overflow_thrown = false;
    try{
        static_cast<void>(delaunay_triangulation({
            {std::numeric_limits<long double>::max(), 0.0L}
        }));
    }catch(const std::overflow_error&){
        overflow_thrown = true;
    }
    assert(overflow_thrown);

    const DelaunayTriangulationResult separated_near_duplicates =
        delaunay_triangulation({
            {0.0L, 0.0L},
            {5.0e-11L, 100.0L},
            {9.0e-11L, 0.0L},
        });
    assert(separated_near_duplicates.sites.size() == 2);
    assert(separated_near_duplicates.representative[0] == 0);
    assert(separated_near_duplicates.representative[2] == 0);

    const DelaunayTriangulationResult coordinate_order_representative =
        delaunay_triangulation({{5.0e-11L, 0.0L}, {0.0L, 0.0L}});
    assert(coordinate_order_representative.sites.size() == 1);
    assert(coordinate_order_representative.sites[0] == 1);
    assert(coordinate_order_representative.representative[0] == 1);
    assert(coordinate_order_representative.representative[1] == 1);

    const DelaunayTriangulationResult small_scale = delaunay_triangulation({
        {0.0L, 0.0L},
        {0.001L, 0.0L},
        {0.001L, 0.002L},
        {0.0L, 0.001L},
    });
    const DelaunayTriangulationResult unit_scale = delaunay_triangulation({
        {0.0L, 0.0L},
        {1.0L, 0.0L},
        {1.0L, 2.0L},
        {0.0L, 1.0L},
    });
    assert(std::find(small_scale.edges.begin(), small_scale.edges.end(),
        std::pair<std::size_t, std::size_t>{1, 3}) != small_scale.edges.end());
    assert(std::find(small_scale.edges.begin(), small_scale.edges.end(),
        std::pair<std::size_t, std::size_t>{0, 2}) == small_scale.edges.end());
    assert(small_scale.edges == unit_scale.edges);
    assert(small_scale.triangles == unit_scale.triangles);

    // 単位円上の点の半径を相対 1e-16 程度だけ揺らした入力。修正前は併合が終わらなかった。
    const std::vector<Point> perturbed_circle{
        {0xd.8f0281fc9f346cap-4L, -0x8.7eb26de8b395aa9p-4L},
        {-0xd.d0585eb0e1e89bep-5L, -0xe.6eb248d83ee680fp-4L},
        {0x9.1a70277f1d61c6fp-4L, 0xd.286c50f3f511f5fp-4L},
        {0xf.ff2bed306b6600ap-4L, -0xa.4c01376fd4182e3p-9L},
        {-0xe.361d471d08f161ep-4L, -0xe.b3b089d5f9866f7p-5L},
        {-0xe.d0765a3b25bf797p-5L, -0xe.2ea303bf0f5b79ep-4L},
        {-0xc.317fa594cd8c4a7p-4L, 0xa.5c114bddada2968p-4L},
        {0xd.82d953ad75c627cp-5L, 0xe.80fddcf70206de4p-4L},
        {-0xc.bd58b61a982f082p-4L, -0x9.ae121f060304cf5p-4L},
        {0xe.b7edbfb470550b2p-4L, 0xc.8c6e8012e9bfb42p-5L},
        {-0xe.311b8290c1c53a6p-6L, -0xf.9a06d4a0a83a8aap-4L},
        {-0xa.1cd5895e02f35adp-4L, 0xc.662366bd0f21f78p-4L},
        {0xf.f9cde4a65d9312fp-4L, 0xe.13455006f51467p-8L},
        {-0xb.78f23e0d914c534p-4L, -0xb.27181af3d22ddfcp-4L},
    };
    verify_floating(perturbed_circle);
    assert(delaunay_triangulation(perturbed_circle).triangles.size() == 12);

    // ほぼ共円・ほぼ共線な点集合をランダムに生成し、厳密な参照実装と比べる。
    std::mt19937_64 random(20261114);
    const auto uniform = [&](long double low, long double high){
        return low + (high - low)
            * std::uniform_real_distribution<long double>(0.0L, 1.0L)(random);
    };
    const long double pi = std::acos(-1.0L);
    for(int iteration = 0; iteration < 3000; ++iteration){
        const int kind = iteration % 4;
        const int size = 3 + static_cast<int>(random() % 30);
        const long double noise = std::array<long double, 5>{
            0.0L, 1.0e-16L, 4.0e-16L, 1.6e-15L, 1.0e-13L
        }[random() % 5];
        std::vector<Point> points;
        if(kind < 2){
            const Point center{uniform(-10.0L, 10.0L), uniform(-10.0L, 10.0L)};
            const long double radius = std::pow(10.0L, uniform(-3.0L, 3.0L));
            const long double rotation = uniform(0.0L, 2.0L * pi);
            for(int index = 0; index < size; ++index){
                const long double angle = kind == 0
                    ? uniform(0.0L, 2.0L * pi)
                    : rotation + 2.0L * pi * index / size;
                const long double current = radius * (1.0L + noise * uniform(-1.0L, 1.0L));
                points.push_back(center + Point{
                    current * std::cos(angle), current * std::sin(angle)
                });
            }
            if(random() % 3 == 0) points.push_back(center);
        }else if(kind == 2){
            const Point origin{uniform(-5.0L, 5.0L), uniform(-5.0L, 5.0L)};
            const long double angle = uniform(0.0L, pi);
            const Point direction{std::cos(angle), std::sin(angle)};
            for(int index = 0; index < size; ++index){
                points.push_back(origin + direction * uniform(-10.0L, 10.0L) + Point{
                    noise * uniform(-1.0L, 1.0L), noise * uniform(-1.0L, 1.0L)
                });
            }
            if(random() % 2 == 0){
                points.push_back({uniform(-10.0L, 10.0L), uniform(-10.0L, 10.0L)});
            }
        }else{
            const long double radius = uniform(0.5L, 2.0L);
            for(int index = 0; index < size; ++index){
                const long double angle = uniform(0.0L, 2.0L * pi);
                const long double current = (index % 2 == 0 ? radius : 2.0L * radius)
                    * (1.0L + noise * uniform(-1.0L, 1.0L));
                points.push_back({current * std::cos(angle), current * std::sin(angle)});
            }
        }
        if(random() % 4 == 0) points.push_back(points[random() % points.size()]);
        verify_floating(points);
    }

    int test_count = 0;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<IntegerPoint> points(size);
        for(auto& [x, y]: points) std::cin >> x >> y;
        verify(points);
        std::cout << "OK\n";
    }
}
