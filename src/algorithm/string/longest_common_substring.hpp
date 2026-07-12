#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "suffix_array.hpp"

struct LongestCommonSubstringResult{
    int first_pos;
    int second_pos;
    int length;
};

LongestCommonSubstringResult longest_common_substring(const std::string& a, const std::string& b){
    const int n = (int)a.size();
    const int m = (int)b.size();
    std::vector<int> s;
    s.reserve(n + 1 + m);
    for(unsigned char c: a){
        s.push_back((int)c + 1);
    }
    s.push_back(0);
    for(unsigned char c: b){
        s.push_back((int)c + 1);
    }

    auto sa = suffix_array(s);
    auto lcp = lcp_array(s, sa);
    LongestCommonSubstringResult best{0, 0, 0};

    auto side = [&](int pos){
        if(pos < n){
            return 0;
        }
        if(n < pos){
            return 1;
        }
        return -1;
    };

    for(int i = 0; i + 1 < (int)sa.size(); i++){
        const int x = side(sa[i]);
        const int y = side(sa[i + 1]);
        if(x < 0 || y < 0 || x == y){
            continue;
        }
        const int len = lcp[i];
        if(best.length < len){
            best.length = len;
            if(x == 0){
                best.first_pos = sa[i];
                best.second_pos = sa[i + 1] - n - 1;
            }else{
                best.first_pos = sa[i + 1];
                best.second_pos = sa[i] - n - 1;
            }
        }
    }
    return best;
}
