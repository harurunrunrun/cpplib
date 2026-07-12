// competitive-verifier: STANDALONE

#include "../generator/partially_persistent_array/generator.inc"
#include "../checker/partially_persistent_array/checker.inc"

int main(){
    generate_partially_persistent_array_standalone_cases();
    run_partially_persistent_array_standalone_check();
}
