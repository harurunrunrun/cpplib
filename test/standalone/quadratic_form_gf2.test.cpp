// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/quadratic_form_gf2.hpp"

using math::QuadraticFormGF2;

namespace{

void self_test(){
    {
        QuadraticFormGF2 form(2);
        form.set_quadratic(0, 1, true);
        const auto canonical = form.canonicalize();
        assert(canonical.bilinear_rank() == 2);
        assert(canonical.arf_invariant() == std::optional<bool>(false));
        assert(canonical.solution_count(false).to_string() == "3");
        assert(canonical.solution_count(true).to_string() == "1");
    }
    {
        QuadraticFormGF2 form(3);
        form.toggle_linear(2);
        const auto canonical = form.canonicalize();
        assert(canonical.is_balanced());
        assert(!canonical.arf_invariant().has_value());
        assert(canonical.solution_count(false).to_string() == "4");
    }
    {
        QuadraticFormGF2 form(1);
        form.toggle_quadratic(0, 0);
        assert(form.linear_coefficient(0));
        assert(form.quadratic_coefficient(0, 0));
    }
    bool threw = false;
    try{
        QuadraticFormGF2 form(1);
        (void)form.evaluate({false, true});
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

void verify_change_of_basis(
    const QuadraticFormGF2& form,
    const math::QuadraticFormGF2Canonical& canonical
){
    if(form.variable_count() > 12) return;
    const std::size_t assignments = std::size_t{1} << form.variable_count();
    for(std::size_t mask = 0; mask < assignments; ++mask){
        std::vector<bool> coordinates(form.variable_count());
        for(std::size_t index = 0; index < form.variable_count(); ++index){
            coordinates[index] = ((mask >> index) & 1U) != 0;
        }
        const std::vector<bool> original =
            canonical.to_original_assignment(coordinates);
        assert(form.evaluate(original) == canonical.evaluate_canonical(coordinates));
    }
}

} // namespace

int main(){
    self_test();
    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::size_t variable_count;
        int constant;
        std::size_t linear_count, quadratic_count;
        std::cin >> variable_count >> constant >> linear_count >> quadratic_count;
        QuadraticFormGF2 form(variable_count);
        form.set_constant(constant != 0);
        for(std::size_t index = 0; index < linear_count; ++index){
            std::size_t variable;
            std::cin >> variable;
            form.toggle_linear(variable);
        }
        for(std::size_t index = 0; index < quadratic_count; ++index){
            std::size_t first, second;
            std::cin >> first >> second;
            form.toggle_quadratic(first, second);
        }
        const auto canonical = form.canonicalize();
        verify_change_of_basis(form, canonical);
        const std::optional<bool> arf = canonical.arf_invariant();
        std::cout << canonical.bilinear_rank() << ' '
                  << canonical.is_balanced() << ' '
                  << (arf.has_value() ? (*arf ? 1 : 0) : -1) << ' '
                  << canonical.solution_count(false) << ' '
                  << canonical.solution_count(true) << '\n';
    }
}
