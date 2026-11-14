#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_SUFFIX_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_SUFFIX_ARRAY_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace suffix_array_internal{

// sa is sorted by the first k elements of each suffix and rank[i] is the class of
// suffix i under that order.  If few suffixes share a class with another one,
// keeps doubling only inside those classes and returns the suffix array;
// otherwise, or if the tied suffixes summed over the rounds exceed n (which keeps
// the work O(n log n)), returns an empty vector.
inline std::vector<int> refine_small_groups(
    std::vector<int> sa, const std::vector<int>& rank, int k
){
    const int n = (int)sa.size();
    std::vector<int> group_rank(n);
    std::vector<std::pair<int, int>> groups;
    int unresolved = 0;
    for(int l = 0, r; l < n; l = r){
        r = l + 1;
        while(r < n && rank[sa[r]] == rank[sa[l]]){
            r++;
        }
        for(int i = l; i < r; i++){
            group_rank[sa[i]] = l;
        }
        if(r - l >= 2){
            groups.emplace_back(l, r);
            unresolved += r - l;
        }
    }
    if(unresolved > n / 64){
        return {};
    }

    std::vector<std::pair<int, int>> keyed(unresolved), next_groups;
    long long budget = n;
    for(; !groups.empty(); k <<= 1){
        int offset = 0;
        for(const auto& [l, r]: groups){
            for(int i = l; i < r; i++){
                const int x = sa[i];
                keyed[offset++] = {x + k < n ? group_rank[x + k] : -1, x};
            }
        }
        budget -= offset;
        if(budget < 0){
            return {};
        }
        offset = 0;
        next_groups.clear();
        for(const auto& [l, r]: groups){
            auto first = keyed.begin() + offset;
            std::sort(first, first + (r - l));
            for(int i = l, j; i < r; i = j){
                j = i + 1;
                while(j < r && keyed[offset + (j - l)].first == keyed[offset + (i - l)].first){
                    j++;
                }
                for(int t = i; t < j; t++){
                    sa[t] = keyed[offset + (t - l)].second;
                    group_rank[sa[t]] = i;
                }
                if(j - i >= 2){
                    next_groups.emplace_back(i, j);
                }
            }
            offset += r - l;
        }
        groups.swap(next_groups);
    }
    return sa;
}

// Prefix doubling.  If max_rounds is given, stops after that many rounds (each
// doubles the compared prefix length) and finishes with refine_small_groups;
// it also gives up early when, after a round, clearly more suffixes are still
// tied than in a random sequence over log_alphabet = log(classes) symbols.
// Returns an empty vector when it gives up.
inline std::vector<int> build_from_ranks(
    std::vector<int> rank, int classes, int max_rounds = -1, double log_alphabet = 0
){
    const int n = (int)rank.size();
    if(n == 0){
        return {};
    }

    std::vector<int> sa(n), idx(n), buf(n), nrank(n), count;
    count.assign(classes, 0);
    for(int x: rank){
        count[x]++;
    }
    for(int i = 1; i < classes; i++){
        count[i] += count[i - 1];
    }
    for(int i = n - 1; i >= 0; i--){
        sa[--count[rank[i]]] = i;
    }

    for(int k = 1; k < n && classes < n; k <<= 1){
        if(max_rounds-- == 0){
            return refine_small_groups(std::move(sa), rank, k);
        }
        std::iota(idx.begin(), idx.end(), 0);

        auto radix_sort = [&](const std::vector<int>& in, std::vector<int>& out, auto key){
            count.assign(classes + 1, 0);
            for(int x: in){
                count[key(x)]++;
            }
            int sum = 0;
            for(int& x: count){
                const int c = x;
                x = sum;
                sum += c;
            }
            for(int x: in){
                out[count[key(x)]++] = x;
            }
        };

        radix_sort(idx, buf, [&](int x){ return x + k < n ? rank[x + k] + 1 : 0; });
        radix_sort(buf, sa, [&](int x){ return rank[x] + 1; });

        nrank[sa[0]] = 0;
        int next_classes = 1, singletons = 0, run = 1;
        for(int i = 1; i < n; i++){
            const int a = sa[i - 1], b = sa[i];
            const int a2 = a + k < n ? rank[a + k] : -1;
            const int b2 = b + k < n ? rank[b + k] : -1;
            if(rank[a] != rank[b] || a2 != b2){
                next_classes++;
                singletons += run == 1;
                run = 0;
            }
            run++;
            nrank[b] = next_classes - 1;
        }
        singletons += run == 1;
        rank.swap(nrank);
        classes = next_classes;
        if(max_rounds >= 0 && classes < n){
            // n balls into alphabet^(2k) bins: the expected number of balls
            // sharing a bin is n (1 - exp(-n / alphabet^(2k))).
            const double load = std::exp(std::log((double)n) - 2.0 * k * log_alphabet);
            const double expected = n * -std::expm1(-load);
            if(n - singletons > 2.0 * expected + n / 64.0){
                return {};
            }
        }
    }
    return sa;
}

inline std::vector<int> sa_is(const std::vector<int>& sequence, int upper);

// Uses prefix doubling (cheaper for large alphabets) when a random sequence over
// the same number of symbols would leave at most n / 64 tied suffixes within a
// few rounds, and SA-IS otherwise or when doubling gives up.  The suffix array
// is unique, so both give the same result.
inline std::vector<int> build_general(std::vector<int> rank, int classes){
    const int n = (int)rank.size();
    if(classes == n){
        return build_from_ranks(std::move(rank), classes);
    }
    if(classes == 1){
        return sa_is(rank, 0);
    }
    const double log_alphabet = std::log((double)classes);
    const double target = std::log(64.0 * n);
    int rounds = 0;
    while(std::ldexp(log_alphabet, rounds) < target){
        rounds++;
    }
    // Rounds affordable before SA-IS becomes faster (measured).
    const int max_rounds = n < (1 << 12) ? 3 : n < (1 << 16) ? 5 : 3;
    if(rounds > max_rounds){
        return sa_is(rank, classes - 1);
    }
    // Counts (or bounds from above) the distinct q-grams, where q = 2 or the largest
    // q with classes^q <= n.  If it is clearly below the expected number for a
    // random sequence, the sequence is repetitive and doubling would need many
    // rounds.
    double distinct = 0, expected = 0;
    if((long long)classes * classes <= n){
        long long grams = classes;
        int q = 1;
        while(grams * classes <= n){
            grams *= classes;
            q++;
        }
        std::vector<char> seen(grams, 0);
        const long long top = grams / classes;  // weight of the oldest symbol
        long long code = 0;
        for(int i = 0; i < n; i++){
            if(i >= q){
                code -= rank[i - q] * top;
            }
            code = code * classes + rank[i];
            if(i >= q - 1 && !seen[code]){
                seen[code] = 1;
                distinct++;
            }
        }
        expected = grams * -std::expm1(-(n - q + 1) / (double)grams);
    }else{
        std::vector<int> first_next(classes, -1);
        distinct = classes;
        for(int i = 0; i + 1 < n; i++){
            int& next = first_next[rank[i]];
            if(next == -1){
                next = rank[i + 1];
            }else if(next != rank[i + 1]){
                distinct++;
            }
        }
        const double pairs = (double)classes * classes;
        expected = pairs * -std::expm1(-(n - 1) / pairs);
    }
    if(distinct < 0.75 * expected){
        return sa_is(rank, classes - 1);
    }
    std::vector<int> sa = build_from_ranks(rank, classes, rounds, log_alphabet);
    if(sa.empty()){
        sa = sa_is(rank, classes - 1);
    }
    return sa;
}

inline std::vector<int> sa_is(const std::vector<int>& sequence, int upper){
    const int size = static_cast<int>(sequence.size());
    if(size == 0) return {};
    if(size == 1) return {0};
    if(size == 2){
        return sequence[0] < sequence[1]
            ? std::vector<int>{0, 1}
            : std::vector<int>{1, 0};
    }
    if(size <= 24){
        std::vector<int> suffixes(static_cast<std::size_t>(size));
        std::iota(suffixes.begin(), suffixes.end(), 0);
        std::sort(suffixes.begin(), suffixes.end(), [&](int left, int right){
            while(left < size && right < size){
                if(sequence[static_cast<std::size_t>(left)] !=
                   sequence[static_cast<std::size_t>(right)]){
                    return sequence[static_cast<std::size_t>(left)] <
                        sequence[static_cast<std::size_t>(right)];
                }
                ++left;
                ++right;
            }
            return left == size && right != size;
        });
        return suffixes;
    }

    std::vector<int> suffixes(static_cast<std::size_t>(size), -1);
    std::vector<bool> is_s(static_cast<std::size_t>(size));
    for(int index = size - 2; index >= 0; --index){
        is_s[static_cast<std::size_t>(index)] =
            sequence[static_cast<std::size_t>(index)] ==
                sequence[static_cast<std::size_t>(index + 1)]
            ? is_s[static_cast<std::size_t>(index + 1)]
            : sequence[static_cast<std::size_t>(index)] <
                sequence[static_cast<std::size_t>(index + 1)];
    }

    std::vector<int> bucket_l(static_cast<std::size_t>(upper + 1));
    std::vector<int> bucket_s(static_cast<std::size_t>(upper + 1));
    for(int index = 0; index < size; ++index){
        if(is_s[static_cast<std::size_t>(index)]){
            bucket_l[static_cast<std::size_t>(
                sequence[static_cast<std::size_t>(index)] + 1
            )]++;
        }else{
            bucket_s[static_cast<std::size_t>(
                sequence[static_cast<std::size_t>(index)]
            )]++;
        }
    }
    for(int value = 0; value <= upper; ++value){
        bucket_s[static_cast<std::size_t>(value)] +=
            bucket_l[static_cast<std::size_t>(value)];
        if(value < upper){
            bucket_l[static_cast<std::size_t>(value + 1)] +=
                bucket_s[static_cast<std::size_t>(value)];
        }
    }

    const auto induce = [&](const std::vector<int>& lms){
        std::fill(suffixes.begin(), suffixes.end(), -1);
        std::vector<int> next = bucket_s;
        for(const int position: lms){
            if(position < size){
                suffixes[static_cast<std::size_t>(
                    next[static_cast<std::size_t>(
                        sequence[static_cast<std::size_t>(position)]
                    )]++
                )] = position;
            }
        }
        next = bucket_l;
        suffixes[static_cast<std::size_t>(
            next[static_cast<std::size_t>(
                sequence[static_cast<std::size_t>(size - 1)]
            )]++
        )] = size - 1;
        for(int index = 0; index < size; ++index){
            const int position = suffixes[static_cast<std::size_t>(index)];
            if(position >= 1 && !is_s[static_cast<std::size_t>(position - 1)]){
                suffixes[static_cast<std::size_t>(
                    next[static_cast<std::size_t>(
                        sequence[static_cast<std::size_t>(position - 1)]
                    )]++
                )] = position - 1;
            }
        }
        next = bucket_l;
        for(int index = size - 1; index >= 0; --index){
            const int position = suffixes[static_cast<std::size_t>(index)];
            if(position >= 1 && is_s[static_cast<std::size_t>(position - 1)]){
                suffixes[static_cast<std::size_t>(
                    --next[static_cast<std::size_t>(
                        sequence[static_cast<std::size_t>(position - 1)] + 1
                    )]
                )] = position - 1;
            }
        }
    };

    std::vector<int> lms_index(static_cast<std::size_t>(size + 1), -1);
    int lms_count = 0;
    for(int index = 1; index < size; ++index){
        if(!is_s[static_cast<std::size_t>(index - 1)] &&
           is_s[static_cast<std::size_t>(index)]){
            lms_index[static_cast<std::size_t>(index)] = lms_count++;
        }
    }
    std::vector<int> lms;
    lms.reserve(static_cast<std::size_t>(lms_count));
    for(int index = 1; index < size; ++index){
        if(lms_index[static_cast<std::size_t>(index)] != -1){
            lms.push_back(index);
        }
    }
    induce(lms);

    if(lms_count != 0){
        std::vector<int> sorted_lms;
        sorted_lms.reserve(static_cast<std::size_t>(lms_count));
        for(const int position: suffixes){
            if(position >= 0 &&
               lms_index[static_cast<std::size_t>(position)] != -1){
                sorted_lms.push_back(position);
            }
        }
        std::vector<int> reduced(static_cast<std::size_t>(lms_count));
        int reduced_upper = 0;
        reduced[static_cast<std::size_t>(
            lms_index[static_cast<std::size_t>(sorted_lms[0])]
        )] = 0;
        for(int index = 1; index < lms_count; ++index){
            int left = sorted_lms[static_cast<std::size_t>(index - 1)];
            int right = sorted_lms[static_cast<std::size_t>(index)];
            const int left_end =
                lms_index[static_cast<std::size_t>(left)] + 1 < lms_count
                ? lms[static_cast<std::size_t>(
                    lms_index[static_cast<std::size_t>(left)] + 1
                )] : size;
            const int right_end =
                lms_index[static_cast<std::size_t>(right)] + 1 < lms_count
                ? lms[static_cast<std::size_t>(
                    lms_index[static_cast<std::size_t>(right)] + 1
                )] : size;
            bool same = left_end - left == right_end - right;
            if(same){
                while(left < left_end &&
                      sequence[static_cast<std::size_t>(left)] ==
                          sequence[static_cast<std::size_t>(right)] &&
                      is_s[static_cast<std::size_t>(left)] ==
                          is_s[static_cast<std::size_t>(right)]){
                    ++left;
                    ++right;
                }
                if(left != left_end ||
                   (left_end != size &&
                    sequence[static_cast<std::size_t>(left)] !=
                        sequence[static_cast<std::size_t>(right)])){
                    same = false;
                }
            }
            if(!same) ++reduced_upper;
            reduced[static_cast<std::size_t>(
                lms_index[static_cast<std::size_t>(
                    sorted_lms[static_cast<std::size_t>(index)]
                )]
            )] = reduced_upper;
        }
        const std::vector<int> reduced_suffixes =
            sa_is(reduced, reduced_upper);
        for(int index = 0; index < lms_count; ++index){
            sorted_lms[static_cast<std::size_t>(index)] =
                lms[static_cast<std::size_t>(
                    reduced_suffixes[static_cast<std::size_t>(index)]
                )];
        }
        induce(sorted_lms);
    }
    return suffixes;
}

} // namespace suffix_array_internal

inline std::vector<int> suffix_array(const std::string& s){
    const int n = (int)s.size();
    if(n == 0){
        return {};
    }

    std::array<int, 256> id;
    id.fill(-1);
    for(unsigned char c: s){
        id[c] = 0;
    }
    int classes = 0;
    for(int c = 0; c < 256; c++){
        if(id[c] == 0){
            id[c] = classes++;
        }
    }

    std::vector<int> rank(n);
    for(int i = 0; i < n; i++){
        rank[i] = id[(unsigned char)s[i]];
    }
    return suffix_array_internal::sa_is(rank, classes - 1);
}

template<class T>
std::vector<int> suffix_array(const std::vector<T>& s){
    const int n = (int)s.size();
    if(n == 0){
        return {};
    }

    std::vector<T> values = s;
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());

    std::vector<int> rank(n);
    for(int i = 0; i < n; i++){
        rank[i] = (int)(std::lower_bound(values.begin(), values.end(), s[i]) - values.begin());
    }
    return suffix_array_internal::build_general(std::move(rank), (int)values.size());
}

template<class Sequence>
std::vector<int> lcp_array(const Sequence& s, const std::vector<int>& sa){
    const int n = (int)s.size();
    if((int)sa.size() != n)[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid argument (lcp_array).");
    }

    std::vector<int> rank(n, -1);
    for(int i = 0; i < n; i++){
        if(sa[i] < 0 || n <= sa[i] || rank[sa[i]] != -1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: invalid argument (lcp_array).");
        }
        rank[sa[i]] = i;
    }

    std::vector<int> lcp(std::max(0, n - 1));
    int h = 0;
    for(int i = 0; i < n; i++){
        const int r = rank[i];
        if(r == 0){
            continue;
        }
        const int j = sa[r - 1];
        while(i + h < n && j + h < n && s[i + h] == s[j + h]){
            h++;
        }
        lcp[r - 1] = h;
        if(h > 0){
            h--;
        }
    }
    return lcp;
}

template<class Sequence>
std::vector<int> lcp_array(const Sequence& s){
    return lcp_array(s, suffix_array(s));
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_SUFFIX_ARRAY_HPP_INCLUDED
