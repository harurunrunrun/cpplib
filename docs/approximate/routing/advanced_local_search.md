---
title: Advanced Routing Local Search (高度巡回路局所探索)
documentation_of: ../../../src/approximate/routing/advanced_local_search.hpp
---

巡回路・複数経路に対する高度な近傍操作と、可変近傍を用いる巡回路改善を提供する。

## 基本交換API

### `adjacent_swap(tour, first)`

`first` と `first + 1` を交換する。時間計算量は $O(1)$。

### `pair_swap(tour, first, second)`

重ならない長さ2のブロックを交換する。時間計算量は $O(1)$。

### `block_swap(tour, first_begin, first_end, second_begin, second_end)`

同じ巡回路内の重ならない2ブロックを交換する。時間・追加空間計算量はともに $O(n)$。

### `cross_exchange(first_route, second_route, ...)`

異なる2経路の指定ブロックを交換する。同じ `vector` を両方の引数へ渡してはならない。時間・追加空間計算量は交換後の2経路長の合計に対して $O(n)$。

### `cyclic_exchange(routes, positions)`

各経路から1頂点を選び、次の経路へ巡回的に移す。時間・追加空間計算量は経路数を $r$ として $O(r)$。

## k-opt API

### `apply_k_opt(tour, cuts, segment_order, reverse_segment_flags)`

`cuts` で分割した全区間を、指定された順序・向きで再接続する一般の k-opt 操作。時間・追加空間計算量は $O(n+k)$。

### `apply_four_opt(tour, a, b, c, d)`

4本の辺を外す double-bridge 型4-optを適用する。時間・追加空間計算量は $O(n)$。

### `apply_double_bridge(tour, a, b, c, d)`

局所最適からの脱出に用いる double-bridge move。時間・追加空間計算量は $O(n)$。

`reverse_segment`、`relocate_segment`、`relocate_vertex`、`swap_vertices` は既存の `local_moves.hpp` がそれぞれ Segment Reversal、Segment Relocation / Node Insertion、Swap を提供する。`apply_or_opt` の `length=1,2,3` が Or-opt-1/2/3 に対応する。

## Ejection API

### `EjectionMove`

移動元経路・位置と、移動先経路・挿入位置を表す。

### `apply_ejection_chain(routes, moves)`

`moves` を順番に実行する ejection chain。経路数を除く総頂点数を $n$、move数を $q$ とすると、`vector` の移動を含む最悪時間計算量は $O(qn)$、追加空間計算量は $O(1)$。

### `EjectionPool<State, Score, Compare>`

容量付き候補集合。`push` は $O(\log p)$、`size` と `empty` は $O(1)$、`sorted` は $O(p\log p)$。ここで $p$ はpool容量。

## `path_relinking(current, target, maximum_steps)`

2つの順列の間を、正しい位置へのswapで結ぶ解列を返す。時間・追加空間計算量は、返却する解列自体を含めて $O(n^2)$。解列を除く作業領域は $O(n)$。

## Lin–Kernighan系API

### `lin_kernighan_tour(distance, tour, maximum_passes)`

全2-opt候補から最良改善を繰り返す、決定的な可変深度局所探索の基礎版。1 passは $O(n^2)$、合計は $O(Pn^2)$、追加空間は $O(n)$。ここで $P$ はpass上限。

### `lin_kernighan_helsgaun_tour(distance, tour, candidate_count, maximum_passes)`

各頂点の近傍候補集合を前計算して探索範囲を絞る版。候補数を $c$ とすると、前計算 $O(n^2\log c)$、改善 $O(Pnc)$、追加空間 $O(nc)$。

## 注意点

距離行列は正方で、巡回路は $[0,n)$ のHamilton閉路を表す順列でなければならない。2-optの差分評価は対称距離と、差分を負数として表現できる距離値型を前提とする。重複するブロック、同一経路同士のcross-exchange、順列でない巡回路など、値の組合せが不正な場合は `std::invalid_argument` を送出する。添字や区間端が範囲外の場合は `std::out_of_range` を送出する。
