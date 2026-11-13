---
title: 1-Tree and Held-Karp Bounds (1-tree・Held–Karp 下界)
documentation_of: ../../../src/approximate/routing/one_tree_heuristics.hpp
---

完全グラフ上の TSP に対して、1-tree と、頂点ペナルティを劣勾配法で更新する Held–Karp 下界を求める。API は `approximate::routing` 名前空間にある。頂点数を $N$ とする。

## `minimum_one_tree`

~~~cpp
OneTreeResult minimum_one_tree(
    const DistanceMatrix& distance,
    size_t root = 0,
    const vector<long double>& vertex_penalty = {}
)
~~~

`root` 以外の頂点の最小全域木に、`root` に接続する最小の 2 辺を加える。ペナルティを $p$ とすると修正後の辺重みは $c(u,v) + p(u) + p(v)$ であり、`lower_bound` は修正後の 1-tree の重みから $2\sum_v p(v)$ を引いた値である。結果には辺、各頂点の次数、修正後の重みも含む。

- 時間計算量: $O(N^2)$
- 追加空間計算量: 返値を含めて $O(N)$

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

各反復で最小の 1-tree を作り、劣勾配 `degree[v] - 2` に沿って頂点ペナルティを更新する。有限の `upper_bound` が現在の下界より大きい場合は Polyak 型のステップ幅を使い、それ以外では劣勾配のノルムで正規化する。
返値は反復中で最大の下界と、それを達成したペナルティ・1-tree である。すべての次数が 2 になれば `is_tour` が真になる。

- 時間計算量: $I$ を `maximum_iterations` として $O(IN^2)$
- 追加空間計算量: $O(N)$

## 注意点

- 距離行列は 3 頂点以上の正方行列を仮定する。
- 頂点数の不足、ペナルティの長さの不一致、有限でないペナルティ、反復数 0、正でないか有限でないステップ係数、NaN の上界に対しては `std::invalid_argument` を送出する。
- 範囲外の `root` には `std::out_of_range` を、$N$ が `INT_MAX` を超える場合は `std::length_error` を送出する。
- TSP の下界として使う場合は、無向で対称な辺重みを渡す。距離の値は `long double` へ変換できなければならない。
