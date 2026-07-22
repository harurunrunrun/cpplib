---
title: Irreducible Pairs from Small Integer Distance Matrix by NTT (NTTによる小整数距離行列の既約対)
documentation_of: ../src/algorithm/graph/specialized/irreducible_pairs_from_small_integer_distance_matrix_ntt.hpp
---

非負整数距離の上限が小さい距離行列について、第三頂点を経由する等長分解を持たない頂点対を列挙する。
距離値ごとの行列積を全組合せで行わず、距離を多項式の指数としてNTTの各評価点で行列を二乗する。
各頂点対が必要とする係数だけを逆変換の直交和で直接復元するため、変換結果を三次元配列として保持しない。

## `irreducible_pairs_from_small_integer_distance_matrix_ntt`

```cpp
vector<pair<int, int>>
irreducible_pairs_from_small_integer_distance_matrix_ntt(
    const vector<vector<long long>>& distances,
    long long maximum_distance
);
```

`left < right` であり、どの `middle != left, right` に対しても

$$
distances[left][right]
\ne distances[left][middle] + distances[middle][right]
$$

となる組を辞書順で返す。異なる頂点間の距離0を許す擬距離にも対応する。
端点自身が作る2個の自明な分解だけが存在する場合を既約と判定する。

変換長を `L = bit_ceil(2 * maximum_distance + 1)` とする。
法998244353の原始 `L` 乗根で距離多項式行列を評価し、それぞれを高速行列積で二乗する。
全ての距離和は `L` 未満なので巡回畳み込みのaliasは起こらず、係数は頂点数未満なので法を取っても中継点数を厳密に復元できる。

## API別の時間計算量・空間計算量

頂点数を $V$、戻り値の組数を $K$、高速行列積の指数を $\omega=\log_2 7$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `irreducible_pairs_from_small_integer_distance_matrix_ntt` | $O(L(V^\omega+V^2)+V^2\log maximum\_distance+K)$ | $O(V^2+K)$ |

`maximum_distance = o(V^{3-\omega})` なら三次時間より漸近的に速い。
距離の種類数を $R$ とする既存の有界整数版 $O(R^2V^\omega)$ と異なり、距離値が密に多数現れる場合でも距離幅に線形である。
上限が大きい場合は通常の三次時間版を使う。

## 注意点

- 行列は正方、対称、対角成分0で、全要素が `[0, maximum_distance]` に入る必要がある。
- 入力は三角不等式を満たす距離または擬距離でなければならない。部分三乗計算量を保つため、三角不等式そのものは検査しない。
- `0 <= maximum_distance < 2^22` が必要である。これによりNTT長は998244353が対応する `2^23` 以下になる。
- 頂点数は998244353未満かつ `int` に収まる必要がある。
- 構造・対称性・値域違反は `invalid_argument`、サイズ表現不能は `length_error` を送出する。
- 単位コストword-RAM上で法演算を定数時間と数える。高速行列積はStrassen法を使用する。
