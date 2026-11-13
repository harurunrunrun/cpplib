---
title: Random and Greedy Max-Cut (ランダム法と貪欲法による最大カット)
documentation_of: ../../../src/approximate/graph/max_cut.hpp
---

無向多重グラフのカットを構築する（`approximate::graph` 名前空間）。自己ループはカットに数えず、平行辺は個別に数える。

## `MaxCutResult`

- `side[v]`: 頂点 $v$ の側（0 または 1）
- `cut_edges`: `side` が切る入力辺の本数

各メンバの参照は時間・追加空間とも $O(1)$、`side` 全体の列挙は時間 $O(N)$、追加空間 $O(1)$ である。

## `cut_edge_count`

~~~cpp
size_t cut_edge_count(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    const vector<unsigned char>& side
)
~~~

指定した 0/1 の割当てが切る辺の本数を数える。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(1)$

## `random_cut`

~~~cpp
template<class URBG>
MaxCutResult random_cut(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges,
    URBG& generator,
    double probability_of_one = 0.5
)
~~~

各頂点を独立に確率 `probability_of_one` で側 1 に置く。デフォルト値では、カットされる辺数の期待値は自己ループを除く辺数の半分である。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(N)$

## `greedy_max_cut`

~~~cpp
MaxCutResult greedy_max_cut(
    size_t vertex_count,
    const vector<pair<size_t, size_t>>& edges
)
~~~

頂点番号順に、割当て済みの隣接頂点への辺をより多く切る側へ置く。同数なら側 0 を選ぶ。自己ループを除く辺の半分以上を決定的に切る。

- 時間計算量: $O(N+E)$
- 追加空間計算量: $O(N+E)$

## 注意点

- 範囲外の端点には `std::out_of_range` を送出する
- `side` の長さが `vertex_count` でない、0, 1 以外の値を含む、または `probability_of_one` が $[0, 1]$ の外にある場合は `std::invalid_argument` を送出する
