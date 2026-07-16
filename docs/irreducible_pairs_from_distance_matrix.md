---
title: Irreducible Pairs from Distance Matrix (距離行列の既約な頂点対) [ROADNET]
documentation_of: ../src/algorithm/graph/irreducible_pairs_from_distance_matrix.hpp
---

有限距離空間の距離行列から、第三の頂点を経由する等長な分解を持たない頂点対を列挙する。

## `irreducible_pairs_from_distance_matrix`

```cpp
vector<pair<int, int>> irreducible_pairs_from_distance_matrix(
    const vector<vector<long long>>& distances
);
```

`left < right` であり、どの `middle != left, right` に対しても

$$
distances[left][right]
\ne distances[left][middle] + distances[middle][right]
$$

となる組だけを辞書順で返す。
この組を無向辺として重み `distances[left][right]` を与えると、入力距離を保つために直接残す必要がある辺集合が得られる。

## API別の時間計算量・空間計算量

頂点数を $N$、戻り値の組数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `irreducible_pairs_from_distance_matrix` | $O(N^3)$ | $O(K)$ |

## 注意点

- 行列は正方、非負、対称、対角成分0で、三角不等式を満たさなければならない。
- 頂点数は `int` に収まる必要がある。
- 前提違反には `invalid_argument` または `length_error` を送出する。
- 距離和は符号付き128 bit整数で比較する。
