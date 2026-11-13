// competitive-verifier: STANDALONE
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>
#include "../../src/algorithm/geometry/2d/result/point_set/smallest_enclosing_annulus.hpp"

namespace{

EnclosingAnnulusResult exhaustive(const std::vector<Point>& points){
    EnclosingAnnulusResult best{
        points.front(), 0.0L,
        std::numeric_limits<long double>::infinity()
    };
    for(const Point point: points){
        smallest_enclosing_annulus_detail::consider(points, point, best);
    }
    for(std::size_t a = 0; a < points.size(); ++a){
        for(std::size_t b = a + 1; b < points.size(); ++b){
            smallest_enclosing_annulus_detail::consider(
                points, (points[a] + points[b]) / 2.0L, best
            );
            for(std::size_t c = b + 1; c < points.size(); ++c){
                if(plus_geometry_detail::orientation(
                    points[a], points[b], points[c]
                ) != 0){
                    smallest_enclosing_annulus_detail::consider(
                        points,
                        smallest_enclosing_annulus_detail::circumcenter(
                            points[a], points[b], points[c]
                        ), best
                    );
                }
            }
            for(std::size_t c = 0; c < points.size(); ++c){
                for(std::size_t d = c + 1; d < points.size(); ++d){
                    const Point first = points[b] - points[a];
                    const Point second = points[d] - points[c];
                    const long double determinant = cross(first, second);
                    if(plus_geometry_detail::sign(determinant) == 0) continue;
                    const long double first_limit =
                        (dot(points[b], points[b]) - dot(points[a], points[a])) / 2.0L;
                    const long double second_limit =
                        (dot(points[d], points[d]) - dot(points[c], points[c])) / 2.0L;
                    const Point center{
                        (first_limit * second.y - first.y * second_limit) / determinant,
                        (first.x * second_limit - first_limit * second.x) / determinant,
                    };
                    smallest_enclosing_annulus_detail::consider(
                        points, center, best
                    );
                }
            }
        }
    }
    return best;
}

}  // namespace

int main(){
    // 単位円上の点の半径を相対 1e-16 程度だけ揺らした入力。修正前は内部の Voronoi 図の
    // 構築が終わらず std::bad_alloc になっていた。
    {
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
        const auto fast = smallest_enclosing_annulus(perturbed_circle);
        const auto slow = exhaustive(perturbed_circle);
        assert(std::abs(
            (fast.outer_radius - fast.inner_radius)
            - (slow.outer_radius - slow.inner_radius)
        ) < 1.0e-12L);
        assert(std::abs(fast.inner_radius - 1.0L) < 1.0e-12L);
    }

    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    std::uint64_t state = 0x243f6a8885a308d3ULL;
    while(cases-- > 0){
        const auto result = smallest_enclosing_annulus({
            {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        });
        for(int random_case = 0; random_case < 100; ++random_case){
            std::vector<Point> points;
            for(int index = 0; index < 7; ++index){
                state ^= state << 7;
                state ^= state >> 9;
                const long double x = static_cast<long double>(
                    static_cast<int>(state % 2001) - 1000
                ) / 41.0L;
                state ^= state << 7;
                state ^= state >> 9;
                const long double y = static_cast<long double>(
                    static_cast<int>(state % 2001) - 1000
                ) / 43.0L;
                points.push_back({x, y});
            }
            const auto fast = smallest_enclosing_annulus(points);
            const auto slow = exhaustive(points);
            const long double fast_width =
                fast.outer_radius - fast.inner_radius;
            const long double slow_width =
                slow.outer_radius - slow.inner_radius;
            assert(std::abs(fast_width - slow_width) < 1.0e-7L);
        }
        const auto two_points = smallest_enclosing_annulus({
            {0, 0}, {3, 0}
        });
        assert(std::abs(
            two_points.outer_radius - two_points.inner_radius
        ) < 1.0e-12L);
        bool rejected_collinear = false;
        try{
            static_cast<void>(smallest_enclosing_annulus({
                {0, 0}, {1, 0}, {3, 0}
            }));
        }catch(const std::domain_error&){
            rejected_collinear = true;
        }
        assert(rejected_collinear);
        assert(std::abs(result.inner_radius - 1) < 1.0e-9L);
        assert(std::abs(result.outer_radius - 1) < 1.0e-9L);
        std::cout << "OK\n";
    }
}
