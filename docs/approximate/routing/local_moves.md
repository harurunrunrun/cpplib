---
title: Routing Local Search Moves (巡回路局所探索近傍)
documentation_of: ../../../src/approximate/routing/local_moves.hpp
---

TSP / VRP の局所探索で使う基本的な近傍操作である。すべて `approximate::routing` 名前空間にあり、区間は半開区間 `[first, last)` で指定する。

## segment reversal / 2-opt

~~~cpp
void reverse_segment(vector<int>& route, size_t first, size_t last)
void apply_two_opt(vector<int>& tour, size_t first, size_t last)
~~~

指定区間を反転する。`apply_two_opt` は同じ操作の 2-opt 用の別名である。

- 時間計算量: $O(\mathrm{last} - \mathrm{first})$
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

対称な距離の巡回路で区間を反転したときのコストの変化 `new_cost - old_cost` を返す。巡回路全体の反転と、長さ 0 または 1 の区間の反転では 0 を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 2-opt star

~~~cpp
void apply_two_opt_star(
    vector<int>& first_route,
    vector<int>& second_route,
    size_t first_cut,
    size_t second_cut
)
~~~

各経路を切断位置の直前で切り、2 本の後半部分（suffix）を交換する。デポを列に含めるかどうかは呼び出し側で統一する。

- 時間計算量: 2 経路の合計長を $L$ として $O(L)$
- 追加空間計算量: 交換する後半部分の合計長に対して $O(L)$

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

2 本の開いた経路で後半部分を交換したときの経路コストの合計の変化 `new_cost - old_cost` を返す。両方の切断位置について直前・直後の頂点が存在する（内部の切断位置である）場合だけを受け付ける。距離は非対称でもよい。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 3-opt

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

`[first_cut, second_cut)` と `[second_cut, third_cut)` の向きと順序を 7 通りに組み替える。切断位置は `0 < first_cut < second_cut < third_cut < tour.size()` を満たさなければならない。

- 時間計算量: 2 区間の合計長を $B$ として $O(B)$
- 追加空間計算量: $O(B)$

## relocate / Or-opt

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

区間を、元の経路における位置 `insert_before` の直前へ移す。挿入位置が移動する区間の内部または直後なら何もしない。Or-opt は長さ 1, 2, 3 だけを受け付ける。

- 各 API の時間計算量: 経路長を $L$ として $O(L)$
- `relocate_segment`, `relocate_vertex`, `apply_or_opt` の追加空間計算量: 移動長を $B$ として $O(B)$

## swap / segment exchange

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

`swap_vertices` は 2 頂点を交換する。`exchange_segments` は重ならない 2 区間を、長さが異なっていても交換する。

- `swap_vertices`: 時間計算量 $O(1)$、追加空間計算量 $O(1)$
- `exchange_segments`: 時間計算量 $O(L)$、追加空間計算量 $O(L)$

## 注意点

- 不正な位置や重なる交換区間には `std::out_of_range` を、Or-opt の不正な長さには `std::invalid_argument` を送出する。
- `two_opt_symmetric_delta` は、正方で対称な距離行列、有効な巡回路の頂点、差分が `Cost` に収まることを前提とする。
