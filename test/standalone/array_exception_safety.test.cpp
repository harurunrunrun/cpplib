// competitive-verifier: STANDALONE

#include "../generator/array_exception_safety/generator.inc"
#include "../checker/array_exception_safety/checker.inc"

int main(){
    generate_array_exception_safety_standalone_cases();
    run_array_exception_safety_standalone_check();
}
