---
title: Minimum-Weight Maximum Unicyclic Matching (単一閉路グラフの最小重み最大マッチング)
documentation_of: ../src/algorithm/matching/minimum_weight_unicyclic_matching.hpp
---

各連結成分が木または $N$ 頂点 $N$ 辺の無向グラフである場合の最小重み最大マッチング。

まずマッチング数を最大化し、その中で重み和を最小化する。

## 関数

```cpp
minimum_weight_unicyclic_matching<T>(n, edges)
```

`edges` は `MinimumWeightUnicyclicMatchingEdge<T>{from, to, cost}` の列。

`MinimumWeightUnicyclicMatchingResult<T>` は次を持つ。

```cpp
int size;
T cost;
vector<int> match;
```

自己ループは扱わない。多重辺は扱える。

## 時間計算量

- `minimum_weight_unicyclic_matching`: $O(N+M)$

各連結成分の検査、木DP・cycle DP、長さ $N$ の `match` 出力を含む。

## 空間計算量

- 戻り値と作業領域を含めて $O(N+M)$

## 注意点

`match[v]` は対応頂点または `-1`。matching数を最大化してからcostを最小化する。成分は木またはunicyclic multigraphとし、多重辺を許すがself-loopや他形状は `runtime_error`。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error`。容量・cost・総和の演算結果が `T` に収まることを前提とする。記載した計算量には引数検査とResultの構築を含む。
