---
title: Routing Local Search Moves
documentation_of: ../src/approximate/routing/local_moves.hpp
---

TSP/VRPの局所探索で使う基本近傍操作。すべて `approximate::routing` 名前空間にあり、区間は半開区間 `[first,last)` で指定する。

# segment reversal / 2-opt

~~~cpp
void reverse_segment(vector<int>& route, size_t first, size_t last)
void apply_two_opt(vector<int>& tour, size_t first, size_t last)
~~~

指定区間を反転する。`apply_two_opt` は同じ操作の2-opt用名称。

- 時間計算量: $O(last-first)$
- 追加空間計算量: $O(1)$

~~~cpp
template<class DistanceMatrix>
Cost two_opt_symmetric_delta(
    const DistanceMatrix& distance,
    const vector<int>& tour,
    size_t first,
    size_t last
)
~~~

対称距離の巡回路で区間反転したときの `new_cost-old_cost` を返す。巡回路全体の反転と長さ0/1の反転は0を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

# 2-opt star

~~~cpp
void apply_two_opt_star(
    vector<int>& first_route,
    vector<int>& second_route,
    size_t first_cut,
    size_t second_cut
)
~~~

各routeをcut直前で切り、2本のsuffixを交換する。depotを列に含めるかどうかは呼び出し側で統一する。

- 時間計算量: 2経路の合計長を $L$ として $O(L)$
- 追加空間計算量: 交換するsuffixの合計長に対して $O(L)$

~~~cpp
template<class DistanceMatrix>
Cost two_opt_star_delta(
    const DistanceMatrix& distance,
    const vector<int>& first_route,
    const vector<int>& second_route,
    size_t first_cut,
    size_t second_cut
)
~~~

2本の開経路でsuffixを交換したときの合計path costの変化 `new_cost-old_cost` を返す。両cutの直前・直後が存在する内部cutだけを受け付ける。距離は非対称でもよい。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

# 3-opt

~~~cpp
enum class ThreeOptPattern {
    reverse_first, reverse_second, reverse_both, exchange,
    reverse_second_exchange, exchange_reverse_first,
    reverse_both_exchange
};

void apply_three_opt(
    vector<int>& tour,
    size_t first_cut,
    size_t second_cut,
    size_t third_cut,
    ThreeOptPattern pattern
)
~~~

`[first_cut,second_cut)` と `[second_cut,third_cut)` の向き・順序を7通りに組み替える。cutは `0 < first < second < third < tour.size()` を満たす必要がある。

- 時間計算量: 2区間の合計長を $B$ として $O(B)$
- 追加空間計算量: $O(B)$

# relocate / Or-opt

~~~cpp
void relocate_segment(
    vector<int>& route,
    size_t first,
    size_t last,
    size_t insert_before
)
void relocate_vertex(
    vector<int>& route,
    size_t vertex_position,
    size_t insert_before
)
void apply_or_opt(
    vector<int>& route,
    size_t first,
    size_t length,
    size_t insert_before
)
~~~

元のrouteにおける `insert_before` の直前へ区間を移す。挿入位置が移動区間内または直後なら何もしない。Or-optは長さ1, 2, 3だけを受け付ける。

- 各APIの時間計算量: route長を $L$ として $O(L)$
- `relocate_segment`, `relocate_vertex`, `apply_or_opt` の追加空間計算量: 移動長を $B$ として $O(B)$

# swap / segment exchange

~~~cpp
void swap_vertices(vector<int>& route, size_t first, size_t second)
void exchange_segments(
    vector<int>& route,
    size_t first_begin,
    size_t first_end,
    size_t second_begin,
    size_t second_end
)
~~~

`swap_vertices` は2頂点を交換する。`exchange_segments` は重ならない2区間を、長さが異なっていても交換する。

- `swap_vertices`: 時間 $O(1)$、追加空間 $O(1)$
- `exchange_segments`: 時間 $O(L)$、追加空間 $O(L)$

# 例外・前提

不正な位置・重なる交換区間には `std::out_of_range`、Or-optの不正な長さには `std::invalid_argument` を送出する。
`two_opt_symmetric_delta` は正方な対称距離行列、有効なtour頂点、差分が `Cost` に収まることを前提とする。
