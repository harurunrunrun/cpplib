// competitive-verifier: STANDALONE

#include "../generator/discrete_log/generator.inc"
#include "../checker/discrete_log/checker.inc"

int main(){
    generate_discrete_log_standalone_cases();
    run_discrete_log_standalone_check();
}
