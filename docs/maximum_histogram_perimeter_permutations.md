---
title: Maximum Histogram Perimeter Permutations (ヒストグラム最大周長と順列数) [HIST2]
documentation_of: ../src/algorithm/other/maximum_histogram_perimeter_permutations.hpp
---

異なる高さの棒を並べたヒストグラムの最大周長と達成順列数を返す。

# HistogramPerimeterPermutations

```cpp
struct HistogramPerimeterPermutations {
    long long maximum_perimeter;
    unsigned long long permutation_count;
};
```

# maximum_histogram_perimeter_permutations

```cpp
HistogramPerimeterPermutations maximum_histogram_perimeter_permutations(
    const vector<int>& heights
);
```

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_histogram_perimeter_permutations` | $O(N^2 2^N)$ | $O(N2^N)$ |

## 注意点

- 高さは正かつ相異なる必要がある。
