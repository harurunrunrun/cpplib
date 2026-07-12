// competitive-verifier: STANDALONE

#include "../generator/compressed_wavelet_matrix/generator.inc"
#include "../checker/compressed_wavelet_matrix/checker.inc"

int main(){
    generate_compressed_wavelet_matrix_standalone_cases();
    run_compressed_wavelet_matrix_standalone_check();
}
