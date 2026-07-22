#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_SUFFIX_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_SUFFIX_ARRAY_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace suffix_array_internal{

inline std::vector<int> build_from_ranks(std::vector<int> rank, int classes){
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
        int next_classes = 1;
        for(int i = 1; i < n; i++){
            const int a = sa[i - 1], b = sa[i];
            const int a2 = a + k < n ? rank[a + k] : -1;
            const int b2 = b + k < n ? rank[b + k] : -1;
            if(rank[a] != rank[b] || a2 != b2){
                next_classes++;
            }
            nrank[b] = next_classes - 1;
        }
        rank.swap(nrank);
        classes = next_classes;
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

std::vector<int> suffix_array(const std::string& s){
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
    return suffix_array_internal::build_from_ranks(std::move(rank), (int)values.size());
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
