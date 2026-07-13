---
title: Tree-Based TSP Approximations (木に基づく巡回セールスマン問題近似)
documentation_of: ../src/approximate/routing/tree_tsp.hpp
---

密な距離行列にPrim法を適用して構築するTSP近似。すべて `approximate::routing` 名前空間にある。

# minimum spanning tree

~~~cpp
template<class DistanceMatrix>
vector<int> minimum_spanning_tree_parents(
    const DistanceMatrix& distance,
    size_t root = 0
)
~~~

完全グラフのMSTを密Prim法で求める。`parent[root] == -1`、それ以外ではMST上の親を返す。同重みでは頂点番号が小さい候補を優先する。

- 時間計算量: $O(N^2)$
- 追加空間計算量: $O(N)$

# Double-Tree / preorder walk

~~~cpp
template<class DistanceMatrix>
vector<int> double_tree_tsp_tour(
    const DistanceMatrix& distance,
    size_t root = 0
)
~~~

MSTのpreorder順に頂点を1回ずつ並べる。対称metric TSPでは、MSTの各辺を2回通るwalkをshortcutしたものに等しく、最適値の2倍以下になる。

- 時間計算量: $O(N^2)$
- 追加空間計算量: $O(N)$

# Christofides

~~~cpp
template<class DistanceMatrix>
vector<int> christofides_tsp_tour(
    const DistanceMatrix& distance,
    size_t root = 0,
    size_t maximum_odd_vertices = 22
)
~~~

MSTの奇数次数頂点上で最小重み完全matchingを厳密な部分集合DPで求め、Euler閉路をshortcutする。対称metric TSPでは最適値の $3/2$ 倍以下になる。

奇数次数頂点数を $M$ とする。

- 時間計算量: $O(N^2 + M^2 2^M)$
- 追加空間計算量: $O(N + 2^M)$

競プロで扱える小規模matching向けであり、`M > maximum_odd_vertices` なら `std::length_error`。`maximum_odd_vertices` は24以下で指定する。既定値22ではDP本体がおよそ $2^{22}$ stateになる。

## 注意点

距離行列は正方で、無向利用時は対称でなければならない。2倍・3/2倍保証には非負、対称、三角不等式をすべて仮定する。非正方行列・不正な上限には `std::invalid_argument`、不正なrootには `std::out_of_range` を送出する。コスト演算結果は要素型に収まらなければならない。
