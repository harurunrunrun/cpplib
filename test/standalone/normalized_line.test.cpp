// competitive-verifier: STANDALONE

#include <concepts>
#include <exception>
#include <type_traits>

#include "../../src/algorithm/geometry/2d/normalized_line_result.hpp"
#include "../../src/algorithm/geometry/2d/normalized_line_line.hpp"
#include "../../src/algorithm/geometry/2d/normalized_line.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    static_assert(std::is_aggregate_v<NormalizedLine>);
    static_assert(std::same_as<decltype(NormalizedLine{}.a), long double>);
    static_assert(std::same_as<decltype(NormalizedLine{}.b), long double>);
    static_assert(std::same_as<decltype(NormalizedLine{}.c), long double>);
    static_assert(std::same_as<
        decltype(normalized_line(Line{})),
        NormalizedLine
    >);

    geometry_test_setup_output();
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Line line;
        std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        try{
            const NormalizedLine result = normalized_line(line);
            geometry_test_print_real(result.a);
            geometry_test_print_real(result.b);
            geometry_test_print_real(result.c);
        }catch(const std::exception&){
            std::cout << "ERR\n";
        }
    }
}
