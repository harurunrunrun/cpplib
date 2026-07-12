// competitive-verifier: STANDALONE

#include "../generator/weighted_wavelet_matrix/generator.inc"
#include "../checker/weighted_wavelet_matrix/checker.inc"

int main(){
    generate_weighted_wavelet_matrix_standalone_cases();
    run_weighted_wavelet_matrix_standalone_check();
}
