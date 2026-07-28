// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/algorithm/geometry/2d/type/definition/planar_point_location.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        PlanarPointLocation locator({
            {{0, 0}, {2, 0}, {2, 2}, {0, 2}},
            {{3, 0}, {5, 0}, {5, 2}, {3, 2}},
        });
        assert(locator.locate({1, 1}).face == 0);
        assert(locator.locate({4, 1}).face == 1);
        assert(locator.locate({2, 1}).on_boundary);
        bool rejected_degenerate = false;
        try{
            static_cast<void>(PlanarPointLocation({
                {{0, 0}, {1, 0}, {2, 0}}
            }));
        }catch(const std::invalid_argument&){
            rejected_degenerate = true;
        }
        assert(rejected_degenerate);
        std::vector<std::vector<Point>> faces;
        for(int index = 0; index < 128; ++index){
            const long double x = 3.0L * index;
            faces.push_back({
                {x, 0}, {x + 2, 0}, {x + 2, 2}, {x, 2}
            });
        }
        PlanarPointLocation many(std::move(faces));
        for(std::size_t index = 0; index < 128; ++index){
            assert(many.locate({3.0L * index + 1, 1}).face == index);
            assert(many.locate({3.0L * index + 2.5L, 1}).face
                == PlanarPointLocationResult::outside);
        }
        assert(locator.locate({8, 1}).face == PlanarPointLocationResult::outside);
        PlanarPointLocation shared({
            {{0, 0}, {2, 0}, {2, 2}, {0, 2}},
            {{2, 0}, {4, 0}, {4, 2}, {2, 2}},
        });
        assert(shared.locate({1, 1}).face == 0);
        assert(shared.locate({3, 1}).face == 1);
        assert(shared.locate({2, 1}).on_boundary);
        assert(shared.locate({2, 0}).on_boundary);
        PlanarPointLocation nested_vertical({
            {{-2, 0}, {0, 0}, {0, 10}, {-2, 10}},
            {{0, 5}, {2, 5}, {2, 6}, {0, 6}},
        });
        const auto long_vertical = nested_vertical.locate({0, 7});
        assert(long_vertical.on_boundary && long_vertical.face == 0);
        const std::vector<std::vector<Point>> irregular_faces{
            {{0, 0}, {4, 0}, {4, 4}, {3, 4},
             {3, 1}, {1, 1}, {1, 4}, {0, 4}},
            {{6, 0}, {9, 1}, {8, 4}, {5, 3}},
        };
        PlanarPointLocation irregular(irregular_faces);
        for(int x = -4; x <= 40; ++x){
            for(int y = -4; y <= 20; ++y){
                const Point query{x / 4.0L, y / 4.0L};
                std::size_t expected = PlanarPointLocationResult::outside;
                bool boundary = false;
                for(std::size_t face = 0;
                    face < irregular_faces.size();
                    ++face){
                    const int relation = plus_geometry_detail::point_in_polygon(
                        irregular_faces[face], query
                    );
                    if(relation != 0) expected = face;
                    boundary = boundary || relation == 1;
                }
                const auto actual = irregular.locate(query);
                assert(actual.face == expected);
                assert(actual.on_boundary == boundary);
            }
        }
        std::cout << "OK\n";
    }
}
