---
title: Count Decreasing Triplets (減少三つ組の個数) [TRIPINV]
documentation_of: ../src/algorithm/other/count_decreasing_triplets.hpp
---

列中の添字 $i<j<k$ かつ $a_i>a_j>a_k$ を満たす三つ組の個数を返す。

## count_decreasing_triplets

```cpp
template<class T, int MAX_SIZE = 100000>
long long count_decreasing_triplets(const vector<T>& values);
```

## 時間計算量

要素数を $N$ として $O(N\log N)$。

## 空間計算量

$O(N)$。

## 注意点

- `T` は全順序を持たなければならない。
- 同値な要素は厳密な大小関係に含めない。
- 異なる値の個数は `MAX_SIZE` 以下でなければならない。
- 結果は `long long` で表現できる必要がある。
