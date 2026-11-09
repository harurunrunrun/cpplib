---
title: Maximum Histogram Perimeter Permutations (ヒストグラム最大周長と順列数) [HIST2]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/assignment_subset/maximum_histogram_perimeter_permutations.hpp
---

高さが相異なる幅 $1$ の棒を並べたヒストグラムについて、周長の最大値と、それを達成する並べ方（順列）の個数を返す。

並び順を $h_{p_0}, \ldots, h_{p_{N-1}}$ とすると、周長は $h_{p_0} + h_{p_{N-1}} + \sum_{i} \lvert h_{p_{i+1}} - h_{p_i} \rvert + 2N$ である。使用済みの棒の集合と最後の棒を状態とするビット DP で計算する。

## HistogramPerimeterPermutations

```cpp
struct HistogramPerimeterPermutations {
    long long maximum_perimeter;
    unsigned long long permutation_count;
};
```

## maximum_histogram_perimeter_permutations

```cpp
HistogramPerimeterPermutations maximum_histogram_perimeter_permutations(
    const std::vector<int>& heights
);
```

`heights` が空なら `{0, 1}` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_histogram_perimeter_permutations` | $O(N^2 2^N)$ | $O(N2^N)$ |

## 注意点

- 高さは正かつ相異なる必要がある。違反時は `std::runtime_error` を送出する。
- $N \le 30$ でなければならず、$N \ge 31$ では `std::runtime_error` を送出する。ただし空間計算量の都合上、実用的な上限はさらに小さい。
- 順列の個数は `unsigned long long` で数え、オーバーフローは検査しない。
