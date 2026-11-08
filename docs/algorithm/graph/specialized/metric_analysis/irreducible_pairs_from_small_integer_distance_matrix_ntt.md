---
title: Irreducible Pairs from Small Integer Distance Matrix by NTT (NTTによる小整数距離行列の既約対)
documentation_of: ../../../../../src/algorithm/graph/specialized/metric_analysis/irreducible_pairs_from_small_integer_distance_matrix_ntt.hpp
---

非負整数距離の上限が小さい距離行列について、第三の頂点を経由する等長な分解を持たない頂点対を列挙する。
距離値ごとの行列積を全組合せで計算するのではなく、距離を多項式の指数とみなし、NTT の各評価点で行列を二乗する。
各頂点対が必要とする係数だけを逆変換の和で直接復元するため、変換結果を三次元配列として保持しない。

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
D[\mathit{left}][\mathit{right}]
\ne D[\mathit{left}][\mathit{middle}] + D[\mathit{middle}][\mathit{right}]
$$

となる組（$D$ は `distances`）を辞書順で返す。異なる頂点間の距離 $0$ を許す擬距離にも対応する。
内部では端点自身も含めて等式を満たす中継点を数え、端点が作る 2 個の自明な分解だけが存在する場合を既約と判定する。

`maximum_distance` を $B$ とし、変換長 $L$ を $2B+1$ 以上の最小の 2 の冪とする。
法 $998244353$ の原始 $L$ 乗根で距離多項式行列を評価し、それぞれを高速行列積で二乗する。
すべての距離和は $L$ 未満なので巡回畳み込みによる折り返しは起こらず、係数は頂点数以下なので、法を取っても中継点数を厳密に復元できる。

## API別の時間計算量・空間計算量

頂点数を $V$、戻り値の組数を $K$、高速行列積の指数を $\omega=\log_2 7$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `irreducible_pairs_from_small_integer_distance_matrix_ntt` | $O(L(V^\omega+V^2)+V^2\log L+K)$ | $O(V^2+K)$ |

$L=O(B)$ なので、$B = o(V^{3-\omega})$ なら三乗時間より漸近的に速い。
距離の種類数を $R$ とする有界整数版の $O(R^2V^\omega)$ と異なり、距離値が密に多数現れる場合でも計算量は距離の上限に線形である。
上限が大きい場合は通常の三乗時間版を使う。

## 注意点

- 行列は正方、対称、対角成分 $0$ で、全要素が閉区間 `[0, maximum_distance]` に入る必要がある。
- 入力は三角不等式を満たす距離または擬距離でなければならない。三乗未満の計算量を保つため、三角不等式そのものは検査しない。
- `0 <= maximum_distance < 2^22` が必要である。これにより NTT の長さは、$998244353$ が対応する $2^{23}$ 以下になる。
- 頂点数は $998244353$ 未満かつ `int` に収まる必要がある。
- 構造・対称性・値域の違反には `invalid_argument`、サイズが表現できない場合には `length_error` を送出する。内部で復元した中継点数の整合性検査に失敗した場合は `logic_error` を送出する。
- 単位コスト word-RAM 上で法演算を定数時間と数える。高速行列積には Strassen 法を使用する。
