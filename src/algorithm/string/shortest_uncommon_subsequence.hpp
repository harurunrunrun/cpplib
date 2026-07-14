#pragma once

#include <algorithm>
#include <vector>

template<class SourceSequence, class ForbiddenSequence>
int shortest_uncommon_subsequence_length(
    const SourceSequence& source,
    const ForbiddenSequence& forbidden
){
    const int n = static_cast<int>(source.size());
    const int m = static_cast<int>(forbidden.size());
    const int infinity = n + 1;

    // next_row[j] is the answer for source[i + 1:] and forbidden[j:].
    std::vector<int> next_row(m + 1, infinity);
    std::vector<int> current_row(m + 1, infinity);
    for(int i = n - 1; i >= 0; i--){
        current_row[m] = 1;
        int next_occurrence = m;
        for(int j = m - 1; j >= 0; j--){
            if(source[i] == forbidden[j]) next_occurrence = j;

            // Either skip source[i], or take it.  If it is taken, the greedy
            // first match in forbidden leaves the largest possible suffix and
            // is therefore the hardest suffix to escape from.
            current_row[j] = next_row[j];
            if(next_occurrence == m){
                current_row[j] = 1;
            }else if(next_row[next_occurrence + 1] < infinity){
                current_row[j] = std::min(
                    current_row[j],
                    next_row[next_occurrence + 1] + 1
                );
            }
        }
        next_row.swap(current_row);
    }
    return next_row[0] < infinity ? next_row[0] : -1;
}
