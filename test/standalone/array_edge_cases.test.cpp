// competitive-verifier: STANDALONE

#include "../generator/array_edge_cases/generator.inc"
#include "../checker/array_edge_cases/checker.inc"

int main(){
    generate_array_edge_cases_standalone_cases();
    run_array_edge_cases_standalone_check();
}
