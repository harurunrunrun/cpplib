---
title: Minimum-Weight Maximum-Cardinality Bipartite Matching (二部グラフの最小重み最大マッチング)
documentation_of: ../../../../src/algorithm/matching/bipartite/minimum_weight_bipartite_matching.hpp
---

二部グラフの最小重み最大マッチングを求める。まずマッチングの大きさを最大化し、その中で重みの和を最小化する。

ソース・左側・右側・シンクの 4 層からなる DAG で初期ポテンシャルを線形時間で作り、各増加路は被約コストに対する Dijkstra 法で求める。残余辺を用いるため、負の入力コストも扱える。

## 関数

```cpp
minimum_weight_bipartite_matching<T>(left_size, right_size, edges, inf = numeric_limits<T>::max() / 4)
```

`edges` は `MinimumWeightBipartiteMatchingEdge<T>{left, right, cost}` の列である。

`MinimumWeightBipartiteMatchingResult<T>` は次のメンバを持つ。

```cpp
int size;
T cost;
vector<int> left_match;
vector<int> right_match;
```

`left_match[l]` と `right_match[r]` は対応する相手側の頂点番号、未マッチなら `-1` である。

`inf` は最短増加路探索での距離の初期値である。到達済みかどうかを別に管理しているため、辺のコストやマッチングのコストが `inf` 以上でも扱える。

## 時間計算量

$V = L + R + 2$、$E' = E + L + R$、$F$ を最終的なマッチングの大きさとする。

- ネットワーク構築と初期ポテンシャル: $O(V + E')$
- 1 回の最短増加路探索: $O((V + E') \log V)$
- `minimum_weight_bipartite_matching` 全体: $O(V + E' + (F + 1)(V + E') \log V)$

$F + 1$ 回目の探索は、増加路が存在しないことを確認するためのものである。$E' = \Omega(V)$ である通常の場合は $O((F + 1) E' \log V)$。長さ $L, R$ の対応列の構築と入力辺の走査もこの上界に含まれる。

## 空間計算量

- 残余グラフ、最短路の作業領域、戻り値を含めて $O(V + E')$

## 注意点

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error` を送出する。コスト、ポテンシャル、距離、重みの和の演算結果が `T` に収まることを前提とする。`T` は加減算、大小比較、および優先度付きキューでの順序比較が可能な型とする。記載した計算量には引数検査と戻り値の構築を含む。
