// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/persistent_wavelet_matrix.hpp"

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        std::vector<int> input(static_cast<std::size_t>(input_n));
        for(int& value: input) std::cin >> value;
        PersistentWaveletMatrix<int, 128, 700, 32, 24> matrix(input);
        auto print_optional = [](std::optional<int> value){
            if(value.has_value()){
                std::cout << *value << '\n';
            }else{
                std::cout << "NONE\n";
            }
        };
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int version, k, value;
                std::cin >> version >> k >> value;
                std::cout << matrix.set(version, k, value) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << matrix.fork(version) << '\n';
            }else if(type == "ACCESS"){
                int version, k;
                std::cin >> version >> k;
                std::cout << matrix.access(version, k) << '\n';
            }else if(type == "RANK"){
                int version, value, l, r;
                std::cin >> version >> value >> l >> r;
                std::cout << matrix.rank(version, value, l, r) << '\n';
            }else if(type == "FREQ"){
                int version, l, r, lower, upper;
                std::cin >> version >> l >> r >> lower >> upper;
                std::cout << matrix.range_freq(
                    version, l, r, lower, upper
                ) << '\n';
            }else if(type == "KTH"){
                int version, l, r, k;
                std::cin >> version >> l >> r >> k;
                std::cout << matrix.kth_smallest(version, l, r, k) << '\n';
            }else if(type == "PREV"){
                int version, l, r, upper;
                std::cin >> version >> l >> r >> upper;
                print_optional(matrix.prev_value(version, l, r, upper));
            }else if(type == "NEXT"){
                int version, l, r, lower;
                std::cin >> version >> l >> r >> lower;
                print_optional(matrix.next_value(version, l, r, lower));
            }else if(type == "SELECT"){
                int version, value, occurrence;
                std::cin >> version >> value >> occurrence;
                std::cout << matrix.select(
                    version, value, occurrence
                ) << '\n';
            }
        }
        return 0;
    }
    constexpr int n = 173;
    constexpr int max_version = 1000;
    std::mt19937 rng(424242);
    std::vector<int> initial(n);
    for(int& value: initial) value = static_cast<int>(rng() % 401) - 200;

    PersistentWaveletMatrix<int, 220, max_version, 32, 24> persistent(initial);
    std::vector<std::vector<int>> versions(1, initial);

    for(int q = 0; q < max_version; q++){
        int base = static_cast<int>(rng() % versions.size());
        int version;
        if(rng() % 5 == 0){
            version = persistent.fork(base);
            versions.push_back(versions[static_cast<std::size_t>(base)]);
        }else{
            int k = static_cast<int>(rng() % n);
            int value = static_cast<int>(rng() % 401) - 200;
            auto next = versions[static_cast<std::size_t>(base)];
            next[k] = value;
            version = persistent.set(base, k, value);
            versions.push_back(std::move(next));
        }
        assert(version == static_cast<int>(versions.size()) - 1);
        assert(persistent.latest_version() == version);

        for(int rep = 0; rep < 10; rep++){
            int v = static_cast<int>(rng() % versions.size());
            int l = static_cast<int>(rng() % (n + 1));
            int r = static_cast<int>(rng() % (n + 1));
            if(l > r) std::swap(l, r);
            int x = static_cast<int>(rng() % 501) - 250;
            int y = static_cast<int>(rng() % 501) - 250;
            if(x > y) std::swap(x, y);

            assert(persistent.rank(v, x, l, r) ==
                std::count(versions[static_cast<std::size_t>(v)].begin() + l,
                    versions[static_cast<std::size_t>(v)].begin() + r,
                    x));
            assert(persistent.range_freq(v, l, r, x) ==
                std::count_if(versions[static_cast<std::size_t>(v)].begin() + l,
                    versions[static_cast<std::size_t>(v)].begin() + r,
                    [&](int value){ return value < x; }));
            assert(persistent.range_freq(v, l, r, x, y) ==
                std::count_if(versions[static_cast<std::size_t>(v)].begin() + l,
                    versions[static_cast<std::size_t>(v)].begin() + r,
                    [&](int value){ return x <= value && value < y; }));

            if(l != r){
                std::vector<int> sorted(
                    versions[static_cast<std::size_t>(v)].begin() + l,
                    versions[static_cast<std::size_t>(v)].begin() + r);
                std::sort(sorted.begin(), sorted.end());
                int k = static_cast<int>(rng() % sorted.size());
                assert(persistent.kth_smallest(v, l, r, k) == sorted[k]);
                assert(persistent.kth_largest(v, l, r, k) == sorted[sorted.size() - 1 - k]);
                auto lower = std::lower_bound(sorted.begin(), sorted.end(), x);
                std::optional<int> previous = lower == sorted.begin()
                    ? std::nullopt : std::optional<int>(*std::prev(lower));
                assert(persistent.prev_value(v, l, r, x) == previous);
                std::optional<int> next = lower == sorted.end()
                    ? std::nullopt : std::optional<int>(*lower);
                assert(persistent.next_value(v, l, r, x) == next);
            }
        }
    }

    for(int x = -220; x <= 220; x++){
        int v = static_cast<int>(rng() % versions.size());
        int occurrence = 0;
        for(int k = 0; k < n; k++){
            if(versions[static_cast<std::size_t>(v)][k] == x){
                assert(persistent.select(v, x, occurrence++) == k);
            }
        }
        assert(persistent.select(v, x, occurrence) == n);
    }

    PartiallyPersistentWaveletMatrix<int, 220, max_version, 32, 24> partial(initial);
    std::vector<std::vector<int>> history(1, initial);
    for(int q = 0; q < max_version; q++){
        int k = static_cast<int>(rng() % n);
        int value = static_cast<int>(rng() % 401) - 200;
        auto next = history.back();
        next[k] = value;
        int version = partial.set(k, value);
        history.push_back(std::move(next));
        assert(version == static_cast<int>(history.size()) - 1);
        assert(partial.latest_version() == version);

        for(int rep = 0; rep < 10; rep++){
            int v = static_cast<int>(rng() % history.size());
            int l = static_cast<int>(rng() % (n + 1));
            int r = static_cast<int>(rng() % (n + 1));
            if(l > r) std::swap(l, r);
            int x = static_cast<int>(rng() % 501) - 250;
            assert(partial.range_freq(v, l, r, x) ==
                std::count_if(history[static_cast<std::size_t>(v)].begin() + l,
                    history[static_cast<std::size_t>(v)].begin() + r,
                    [&](int value){ return value < x; }));
            if(l != r){
                std::vector<int> sorted(
                    history[static_cast<std::size_t>(v)].begin() + l,
                    history[static_cast<std::size_t>(v)].begin() + r);
                std::sort(sorted.begin(), sorted.end());
                int kth = static_cast<int>(rng() % sorted.size());
                assert(partial.kth_smallest(v, l, r, kth) == sorted[kth]);
            }
        }
    }

    std::vector<unsigned> small = {0, 1, 7, 3, 12, 31, 4, 8};
    PersistentWaveletMatrix<unsigned, 16, 4, 5, 4> small_matrix(small);
    int small_version = small_matrix.set(0, 2, 15U);
    assert(small_matrix.access(0, 2) == 7U);
    assert(small_matrix.access(small_version, 2) == 15U);

    bool thrown = false;
    try{
        small_matrix.set(0, 0, 32U);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
