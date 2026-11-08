---
title: Irreducible Pairs from Bounded Integer Distance Matrix (有界整数距離行列の既約な頂点対)
documentation_of: ../../../../../src/algorithm/graph/specialized/metric_analysis/irreducible_pairs_from_bounded_integer_distance_matrix.hpp
---

有限の非負整数距離が少数の値しか取らない距離行列から、第三の頂点を経由する等長な分解を持たない頂点対を高速に列挙する。
一般の min-plus 積を高速化する API ではなく、距離値の種類数が少ない入力専用の API である。

## `irreducible_pairs_from_bounded_integer_distance_matrix`

```cpp
vector<pair<int, int>>
irreducible_pairs_from_bounded_integer_distance_matrix(
    const vector<vector<long long>>& distances,
    long long maximum_distance
);
```

全距離が閉区間 `[0, maximum_distance]` に入ることを検査したうえで、`left < right` かつ、どの `middle != left, right` に対しても

$$
D[\mathit{left}][\mathit{right}]
\ne D[\mathit{left}][\mathit{middle}] + D[\mathit{middle}][\mathit{right}]
$$

となる組（$D$ は `distances`）を辞書順で返す。`left` と `right` 自身は中継点として数えない。異なる頂点間の距離 $0$ を持つ頂点も第三の中継点になり得るため、擬距離にも正しく対応する。

距離値ごとに「距離が $a$ である成分」を表す 0/1 行列を作り、和が最大距離以下になる距離値の組 $(a, b)$ について Strassen 法で行列積を計算する。積の成分は、距離和が $a+b$ になる中継点の個数である。計数は法 $2^{61}-1$ で行うが、各成分の真の値は頂点数以下で法より小さいので、$0$ 判定と中継点数はともに厳密である。

## API別の時間計算量・空間計算量

頂点数を $V$、実際に現れる距離値の種類数を $R$、戻り値の組数を $K$、$\omega=\log_2 7$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `irreducible_pairs_from_bounded_integer_distance_matrix` | $O(V^2 \log V + R^2 V^\omega + K)$ | $O(V^2 + K)$ |

`maximum_distance` を $B$ とすると $R \le B+1$ である。特に $B=O(1)$、すなわち距離の上限が頂点数によらない定数なら $O(V^\omega+K)$ となり、通常版の $O(V^3)$ より漸近的に速い。$R$ が大きい入力では通常版より遅くなり得るため、一般の距離行列には `irreducible_pairs_from_distance_matrix` を使う。

## 注意点

- 行列は正方、対称、対角成分 $0$ で、三角不等式を満たす必要がある。
- 距離は有限の非負整数で、`maximum_distance` 以下でなければならない。負の値、上限を超える値は受理しない。`maximum_distance` が負または `LLONG_MAX`（無限大表現とみなす）の場合も受理しない。
- 頂点数は `int` に収まる必要がある。
- 構造・距離範囲・三角不等式の違反には `invalid_argument`、大きさが表現できない場合には `length_error` を送出する。
- 距離の加算比較には符号付き 128 bit 整数を使うため、`long long` の加算オーバーフローは起きない。
- GCC の GNU 拡張である 128 bit 整数（`__int128_t`）を使用する。
