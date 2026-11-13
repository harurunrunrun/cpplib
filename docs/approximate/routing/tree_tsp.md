---
title: Tree-Based TSP Approximations (木に基づく巡回セールスマン問題近似)
documentation_of: ../../../src/approximate/routing/tree_tsp.hpp
---

密な距離行列に Prim 法を適用して作る TSP 近似である。すべて `approximate::routing` 名前空間にある。頂点数を $N$ とする。

## minimum spanning tree

~~~cpp
template<class DistanceMatrix>
vector<int> minimum_spanning_tree_parents(
    const DistanceMatrix& distance,
    size_t root = 0
)
~~~

完全グラフの最小全域木を密グラフ用の Prim 法で求める。`parent[root] == -1` で、それ以外の頂点には最小全域木上の親を返す。重みが同じ場合は頂点番号が小さい候補を優先する。

- 時間計算量: $O(N^2)$
- 追加空間計算量: $O(N)$

## Double-Tree / preorder walk

~~~cpp
template<class DistanceMatrix>
vector<int> double_tree_tsp_tour(
    const DistanceMatrix& distance,
    size_t root = 0
)
~~~

最小全域木の行きがけ順に、各頂点を 1 回ずつ並べる。対称な metric TSP では、最小全域木の各辺を 2 回通る閉路を近道（shortcut）したものに等しく、コストは最適値の 2 倍以下になる。

- 時間計算量: $O(N^2)$
- 追加空間計算量: $O(N)$

## Christofides

~~~cpp
template<class DistanceMatrix>
vector<int> christofides_tsp_tour(
    const DistanceMatrix& distance,
    size_t root = 0,
    size_t maximum_odd_vertices = 22
)
~~~

最小全域木の奇数次数頂点上の最小重み完全マッチングを厳密な部分集合 DP で求め、得られたオイラー閉路を近道する。対称な metric TSP では、コストは最適値の $3/2$ 倍以下になる。

奇数次数頂点の数を $M$ とする。

- 時間計算量: $O(N^2 + M 2^M)$
- 追加空間計算量: $O(N + 2^M)$

小規模なマッチング向けであり、`M > maximum_odd_vertices` なら `std::length_error` を送出する。`maximum_odd_vertices` は 24 以下で指定する。デフォルト値 22 では DP の状態数がおよそ $2^{22}$ になる。

## 注意点

- 距離行列は正方で、無向グラフとして使うなら対称でなければならない。2 倍・$3/2$ 倍の保証には、非負性・対称性・三角不等式をすべて仮定する。
- 正方でない行列や 24 を超える `maximum_odd_vertices` には `std::invalid_argument` を、範囲外の `root` には `std::out_of_range` を送出する。
- コストの演算結果は要素型に収まらなければならない。
