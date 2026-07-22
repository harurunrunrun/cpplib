---
title: Irreducible Pairs from Bounded Integer Distance Matrix (有界整数距離行列の既約な頂点対)
documentation_of: ../src/algorithm/graph/specialized/irreducible_pairs_from_bounded_integer_distance_matrix.hpp
---

有限の非負整数距離が少数の値しか取らない距離行列から、第三の頂点を経由する等長な分解を持たない頂点対を高速に列挙する。
一般の min-plus 積を高速化するAPIではなく、距離値の種類数が少ない入力専用のAPIである。

## `irreducible_pairs_from_bounded_integer_distance_matrix`

```cpp
vector<pair<int, int>>
irreducible_pairs_from_bounded_integer_distance_matrix(
    const vector<vector<long long>>& distances,
    long long maximum_distance
);
```

全距離が閉区間 `[0, maximum_distance]` に入ることを検査し、`left < right` かつ、どの `middle != left, right` に対しても

$$
distances[left][right]
\ne distances[left][middle] + distances[middle][right]
$$

となる組を辞書順で返す。`left` と `right` 自身は中継点として数えない。距離0の異なる頂点は第三の中継点になり得るため、擬距離にも正確に対応する。

距離が `a` である成分の0/1行列を距離値ごとに作り、距離値 `a, b` の各組について Strassen 行列積を行う。積の成分は、距離和が `a+b` になる中継点数である。計数は法 $2^{61}-1$ で行うが、頂点数は法より小さく、各成分の真の値は頂点数以下なので、0判定と中継点数はともに厳密である。

## API別の時間計算量・空間計算量

頂点数を $V$、実際に現れる距離値の種類数を $R$、戻り値の組数を $K$、$\omega=\log_2 7$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `irreducible_pairs_from_bounded_integer_distance_matrix` | $O(V^2 \log V + R^2 V^\omega + K)$ | $O(V^2 + K)$ |

`maximum_distance = B` なら $R \le B+1$ である。特に $B=O(1)$、すなわち距離上限が頂点数によらない定数なら $O(V^\omega+K)$ であり、通常版の $O(V^3)$ より漸近的に速い。$R$ が大きい入力では通常版より遅くなり得るため、一般距離行列には `irreducible_pairs_from_distance_matrix` を使う。

## 注意点

- 行列は正方、対称、対角成分0で、三角不等式を満たす必要がある。
- 距離は有限の非負整数で、`maximum_distance` 以下でなければならない。負値、上限外の値、`LLONG_MAX` を上限とする無限大表現は受理しない。
- 頂点数は `int` に収まる必要がある。
- 構造、距離範囲、三角不等式の違反には `invalid_argument`、大きさの表現不能には `length_error` を送出する。
- 距離値の加算比較には符号付き128 bit整数を使うため、`long long` の加算オーバーフローは起きない。
- GCC 13のGNU拡張である128 bit整数を使用する。
