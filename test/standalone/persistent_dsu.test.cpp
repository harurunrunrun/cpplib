// competitive-verifier: STANDALONE

#include "../generator/persistent_dsu/generator.inc"
#include "../checker/persistent_dsu/checker.inc"

int main(){
    generate_persistent_dsu_standalone_cases();
    run_persistent_dsu_standalone_check();
}
