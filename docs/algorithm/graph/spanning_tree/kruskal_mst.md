---
title: Kruskal MST (クラスカル法) [GRL_2_A] [minimum_spanning_tree]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/kruskal_mst.hpp
---

Kruskal 法で最小全域木を求める。

## 関数

```cpp
template <class T>
KruskalMstResult<T> kruskal_mst(int n, const std::vector<KruskalEdge<T>>& edges)
```

`edges` は `KruskalEdge<T>{from, to, cost}` の列である。

戻り値 `KruskalMstResult<T>` は次のメンバをもつ。

```cpp
T cost;
std::vector<int> edge_ids;
bool connected;
```

- `cost`: 採用した辺のコストの総和。
- `edge_ids`: 採用した辺の、`edges` における添字（コストの昇順、同コストは入力順）。
- `connected`: グラフが連結なら `true`。

非連結なら最小全域森を返し、`connected` は `false` になる。

## 時間計算量

頂点数を $N$、辺数を $M$ として $O(N + M\log(M + 1))$。

辺の添字の安定ソート、Union-Find の処理、最大 $N-1$ 個の辺の添字の出力を含む。

## 空間計算量

戻り値、ソート順、Union-Find を含めて $O(N + M)$。

## 注意点

`n` が負、または辺の端点が $[0, n)$ の範囲外なら `runtime_error` を送出する。
