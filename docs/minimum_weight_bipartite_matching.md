---
title: Minimum-Weight Maximum-Cardinality Bipartite Matching (二部グラフの最小重み最大マッチング)
documentation_of: ../src/algorithm/matching/minimum_weight_bipartite_matching.hpp
---

二部グラフの最小重み最大マッチング。

まずマッチング数を最大化し、その中で重み和を最小化する。

source、左側、右側、sinkの4層DAGから初期potentialを線形時間で作り、
各増加路はreduced costに対するDijkstra法で求める。残余辺を使うため、
負の入力costも扱える。

## 関数

```cpp
minimum_weight_bipartite_matching<T>(left_size, right_size, edges, inf)
```

`edges` は `MinimumWeightBipartiteMatchingEdge<T>{left, right, cost}` の列。

`MinimumWeightBipartiteMatchingResult<T>` は次を持つ。

```cpp
int size;
T cost;
vector<int> left_match;
vector<int> right_match;
```

`inf` は最短増加路探索の初期値であり、辺コストやマッチングのコストが `inf` 以上でも扱える。

増加路とマッチングのコストは `T` で表現できる必要がある。

## 時間計算量

$V=L+R+2$、$E'=E+L+R$、$F$ を最終matching数とする。

- network構築と初期potential: $O(V+E')$
- 1回の最短増加路探索: $O((V+E')\log V)$
- `minimum_weight_bipartite_matching` 全体:
  $O(V+E' + (F+1)(V+E')\log V)$

$F+1$ 回目は増加路が存在しないことを確認する探索である。
$E'=\Omega(V)$ の通常の表現では $O((F+1)E'\log V)$。
長さ $L,R$ の対応列構築と入力辺の走査も上界に含む。

## 空間計算量

- 残余グラフ、最短路作業領域、戻り値を含めて $O(V+E')$

## 注意点

`left_match[l]` と `right_match[r]` は対応端点、未matchingなら `-1`。端点範囲を検査する。負costを扱える。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は
`runtime_error`。cost、potential、距離、重み和の演算結果が `T` に
収まることを前提とする。`T` は加減算、大小比較および優先度付きキューの
順序比較が可能な型とする。記載した計算量には引数検査とResultの構築を含む。
