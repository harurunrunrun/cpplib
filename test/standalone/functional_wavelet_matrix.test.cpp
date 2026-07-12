// competitive-verifier: STANDALONE

#include "../generator/functional_wavelet_matrix/generator.inc"
#include "../checker/functional_wavelet_matrix/checker.inc"

int main(){
    generate_functional_wavelet_matrix_standalone_cases();
    run_functional_wavelet_matrix_standalone_check();
}
