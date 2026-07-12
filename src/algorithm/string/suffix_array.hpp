#pragma once

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
    return suffix_array_internal::build_from_ranks(std::move(rank), classes);
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
