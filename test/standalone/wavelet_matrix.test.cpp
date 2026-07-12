// competitive-verifier: STANDALONE

#include "../generator/wavelet_matrix/generator.inc"
#include "../checker/wavelet_matrix/checker.inc"

int main(){
    generate_wavelet_matrix_standalone_cases();
    run_wavelet_matrix_standalone_check();
}
