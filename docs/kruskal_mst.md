---
title: Kruskal MST (クラスカル法) [BLINNET] [GRL_2_A] [minimum_spanning_tree]
documentation_of: ../src/algorithm/graph/kruskal_mst.hpp
---

Kruskal法で最小全域木を求める。

# 関数

```cpp
kruskal_mst<T>(n, edges)
```

`edges` は `KruskalEdge<T>{from, to, cost}` の列。

`KruskalMstResult<T>` は次を持つ。

```cpp
T cost;
vector<int> edge_ids;
bool connected;
```

非連結なら最小全域森を返し、`connected` は `false`。

## 時間計算量

- `kruskal_mst`: $O(N+M\log(M+1))$

辺IDのstable sort、DSU処理、最大 $N-1$ 個の辺ID出力を含む。

## 空間計算量

- 戻り値、整列順、DSUを含めて $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
