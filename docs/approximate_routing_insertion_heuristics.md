---
title: Routing Construction and Insertion Heuristics (巡回路構築・挿入ヒューリスティック)
documentation_of: ../src/approximate/routing/insertion_heuristics.hpp
---

完全距離行列からTSP巡回路を決定的に構築する軽量heuristic。すべて `approximate::routing` 名前空間にあり、返す列は始点を末尾に重複して持たない。

# nearest neighbor

~~~cpp
template<class DistanceMatrix>
vector<int> nearest_neighbor_tour(
    const DistanceMatrix& distance,
    size_t start = 0
)
~~~

現在頂点から最も近い未訪問頂点を順に選ぶ。同値なら頂点番号が小さい方を選ぶ。

- 時間計算量: $O(N^2)$
- 追加空間計算量: $O(N)$

~~~cpp
template<class DistanceMatrix>
vector<int> repeated_nearest_neighbor_tour(const DistanceMatrix& distance)
~~~

全頂点を始点として最近傍法を実行し、巡回路コストが最小のものを返す。同値なら始点番号が小さいものを残す。

- 時間計算量: $O(N^3)$
- 追加空間計算量: $O(N)$

# nearest / cheapest / farthest insertion

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

- nearest insertion: 現在のtourからの最短距離が最小の頂点を、コスト増分最小位置へ挿入する。
- cheapest insertion: 全未訪問頂点・全挿入位置のうちコスト増分が最小の組を選ぶ。
- farthest insertion: 現在のtourからの最短距離が最大の頂点を、コスト増分最小位置へ挿入する。

nearest/farthestの頂点選択には `distance[tour_vertex][candidate]` を使う。tieは頂点番号、次に挿入位置が小さいものを優先する。

- `nearest_insertion_tour`, `farthest_insertion_tour`: 時間計算量は $O(N^2)$、追加空間 $O(N)$
- `cheapest_insertion_tour`: 時間計算量は $O(N^3)$、追加空間 $O(N)$

# regret insertion

~~~cpp
template<class DistanceMatrix>
vector<int> regret_insertion_tour(
    const DistanceMatrix& distance,
    size_t start = 0,
    size_t regret_k = 2
)
~~~

各未訪問頂点について最良挿入と第 `min(regret_k,current_tour_size)` 候補の増分差を求め、そのregretが最大の頂点を最良位置へ挿入する。`regret_k=2`, `3` がRegret-2, Regret-3に対応する。

- 時間計算量: $O(N^3\log N)$
- 追加空間計算量: $O(N)$

## 注意点

`distance` は正方行列でなければならず、違反時は `std::invalid_argument`。非空行列で `start >= N` の場合は `std::out_of_range`。`regret_k=0` は `std::invalid_argument`。
距離の和・差は行列要素型に収まらなければならない。近似保証には対称性・三角不等式など別の仮定が必要であり、これらのheuristic自体は保証を持たない。
