// competitive-verifier: STANDALONE

#include "../generator/wavelet_matrix_2d/generator.inc"
#include "../checker/wavelet_matrix_2d/checker.inc"

int main(){
    generate_wavelet_matrix_2d_standalone_cases();
    run_wavelet_matrix_2d_standalone_check();
}
