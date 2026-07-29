---
title: 1-Tree and Held-Karp Bounds (1-tree・Held–Karp下界)
documentation_of: ../../../src/approximate/routing/one_tree_heuristics.hpp
---

完全グラフのTSPに対する1-treeと、頂点penaltyをsubgradient法で更新するHeld–Karp下界を求める。APIは `approximate::routing` 名前空間にある。

## `minimum_one_tree`

~~~cpp
OneTreeResult minimum_one_tree(
    const DistanceMatrix& distance,
    size_t root = 0,
    const vector<long double>& vertex_penalty = {}
)
~~~

`root` 以外の頂点の最小全域木に、`root` に接続する最小の2辺を加える。修正辺重みは
$c(u,v)+p(u)+p(v)$ であり、`lower_bound` は修正後の1-tree重みから
$2\sum_v p(v)$ を引いた値である。結果には辺、各頂点の次数、修正後重みも含む。

- 時間計算量: $O(N^2)$
- 追加空間計算量: $O(N)$（返り値を含めても $O(N)$）

## `held_karp_subgradient_heuristic`

~~~cpp
HeldKarpSubgradientResult held_karp_subgradient_heuristic(
    const DistanceMatrix& distance,
    long double upper_bound,
    size_t maximum_iterations = 200,
    long double initial_step_factor = 2,
    size_t root = 0
)
~~~

各反復で最小1-treeを作り、subgradient `degree[v] - 2` に沿って頂点penaltyを更新する。有限の `upper_bound` が現在の下界より大きい場合はPolyak型stepを使い、それ以外ではsubgradientのnormで正規化する。返り値は反復中で最大の下界と、それを達成したpenalty・1-treeである。全次数が2になれば `is_tour` が真になる。

- 時間計算量: $O(IN^2)$（`I = maximum_iterations`）
- 追加空間計算量: $O(N)$

## 注意点

距離行列は3頂点以上の正方行列を仮定する。頂点数不足、penalty長不一致、反復数0、非正または非有限なstep係数、NaNの上界は `std::invalid_argument`、範囲外のrootは `std::out_of_range`。

Non-finite `vertex_penalty`: `std::invalid_argument`. `N > INT_MAX`: `std::length_error`.
TSP下界として使う場合は無向対称な辺重みを渡す。入力値から `long double` への変換が可能でなければならない。
