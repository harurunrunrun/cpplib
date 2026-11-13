---
title: Routing Construction and Insertion Heuristics (巡回路構築・挿入ヒューリスティック)
documentation_of: ../../../src/approximate/routing/insertion_heuristics.hpp
---

完全グラフの距離行列から TSP の巡回路を決定的に構築する軽量なヒューリスティックである。すべて `approximate::routing` 名前空間にあり、返す列は始点を末尾に重複して持たない。頂点数を $N$ とする。

## nearest neighbor

~~~cpp
template<class DistanceMatrix>
vector<int> nearest_neighbor_tour(
    const DistanceMatrix& distance,
    size_t start = 0
)
~~~

現在の頂点から最も近い未訪問頂点を順に選ぶ。同値なら頂点番号が小さい方を選ぶ。

- 時間計算量: $O(N^2)$
- 追加空間計算量: $O(N)$

~~~cpp
template<class DistanceMatrix>
vector<int> repeated_nearest_neighbor_tour(const DistanceMatrix& distance)
~~~

すべての頂点を始点として最近傍法を実行し、巡回路のコストが最小のものを返す。同値なら始点番号が小さいものを残す。

- 時間計算量: $O(N^3)$
- 追加空間計算量: $O(N)$

## nearest / cheapest / farthest insertion

~~~cpp
template<class DistanceMatrix>
vector<int> nearest_insertion_tour(
    const DistanceMatrix& distance,
    size_t start = 0
)

template<class DistanceMatrix>
vector<int> cheapest_insertion_tour(
    const DistanceMatrix& distance,
    size_t start = 0
)

template<class DistanceMatrix>
vector<int> farthest_insertion_tour(
    const DistanceMatrix& distance,
    size_t start = 0
)
~~~

- nearest insertion: 現在の巡回路からの最短距離が最小の頂点を、コストの増分が最小の位置へ挿入する。
- cheapest insertion: すべての未訪問頂点と挿入位置の組のうち、コストの増分が最小の組を選ぶ。
- farthest insertion: 現在の巡回路からの最短距離が最大の頂点を、コストの増分が最小の位置へ挿入する。

nearest / farthest の頂点選択には `distance[tour_vertex][candidate]` を使う。同値の場合は頂点番号、次に挿入位置が小さいものを優先する。

- `nearest_insertion_tour`, `farthest_insertion_tour`: 時間計算量 $O(N^2)$、追加空間計算量 $O(N)$
- `cheapest_insertion_tour`: 時間計算量 $O(N^3)$、追加空間計算量 $O(N)$

## arbitrary insertion

~~~cpp
template<class DistanceMatrix>
vector<int> arbitrary_insertion_tour(
    const DistanceMatrix& distance,
    const vector<int>& insertion_order
)

template<class DistanceMatrix>
vector<int> arbitrary_insertion_tour(const DistanceMatrix& distance)
~~~

`insertion_order` の先頭の頂点から巡回路を始め、以降の頂点を指定順に、コストの増分が最小の位置へ挿入する任意挿入法である。同値な挿入位置では添字が小さい位置を選ぶ。引数を省略したオーバーロードは、頂点を $0, 1, \ldots, N-1$ の順に挿入する。

- 時間計算量: $O(N^2)$
- 追加空間計算量: $O(N)$

## regret insertion

~~~cpp
template<class DistanceMatrix>
vector<int> regret_insertion_tour(
    const DistanceMatrix& distance,
    size_t start = 0,
    size_t regret_k = 2
)
~~~

各未訪問頂点について、最良の挿入と第 `min(regret_k, current_tour_size)` 候補の挿入との増分の差（regret）を求め、regret が最大の頂点を最良の位置へ挿入する。`regret_k = 2`, `3` がそれぞれ Regret-2, Regret-3 に対応する。

- 時間計算量: $O(N^3\log N)$
- 追加空間計算量: $O(N)$

## 注意点

- `distance` は正方行列でなければならず、違反時は `std::invalid_argument` を送出する。空でない行列で `start >= N` の場合は `std::out_of_range` を、`regret_k == 0` の場合は `std::invalid_argument` を送出する。
- 任意挿入法の `insertion_order` は $[0, N)$ の全頂点をちょうど 1 回ずつ含まなければならない。長さの不一致や重複には `std::invalid_argument` を、範囲外の頂点には `std::out_of_range` を送出する。
- 距離の和・差は行列の要素型に収まらなければならない。これらのヒューリスティック自体は近似保証を持たない。
