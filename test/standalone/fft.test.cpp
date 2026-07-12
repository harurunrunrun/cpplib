// competitive-verifier: STANDALONE

#include "../generator/fft/generator.inc"
#include "../checker/fft/checker.inc"

int main(){
    generate_fft_standalone_cases();
    run_fft_standalone_check();
}
