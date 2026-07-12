// competitive-verifier: STANDALONE

#include "../generator/exponentiable_modint/generator.inc"
#include "../checker/exponentiable_modint/checker.inc"

int main(){
    generate_exponentiable_modint_standalone_cases();
    run_exponentiable_modint_standalone_check();
}
