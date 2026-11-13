---
title: Advanced Routing Local Search (高度な巡回路局所探索)
documentation_of: ../../../src/approximate/routing/advanced_local_search.hpp
---

巡回路や複数経路に対する高度な近傍操作と、可変深度の巡回路改善を提供する。頂点数（経路長）を $n$ とする。

## 基本交換 API

### `adjacent_swap(tour, first)`

位置 `first` と `first + 1` を交換する。時間計算量は $O(1)$。

### `pair_swap(tour, first, second)`

重ならない長さ 2 のブロックを交換する。時間計算量は $O(1)$。

### `block_swap(tour, first_begin, first_end, second_begin, second_end)`

同じ巡回路内の重ならない 2 ブロックを交換する。時間計算量・追加空間計算量はともに $O(n)$。

### `cross_exchange(first_route, second_route, first_begin, first_end, second_begin, second_end)`

異なる 2 経路の指定ブロックを交換する。同じ `vector` を両方の引数に渡してはならない。時間計算量・追加空間計算量は、交換後の 2 経路の長さの合計を $n$ として $O(n)$。

### `cyclic_exchange(routes, positions)`

各経路から 1 頂点ずつ選び、次の経路へ巡回的に移す。経路数を $r$ として、時間計算量・追加空間計算量は $O(r)$。

## k-opt API

### `apply_k_opt(tour, cuts, segment_order, reverse_segment_flags)`

`cuts` で分割したすべての区間を、指定した順序・向きで再接続する一般の k-opt 操作。時間計算量・追加空間計算量は $O(n + k)$。

### `apply_four_opt(tour, a, b, c, d)`

4 本の辺を外す double-bridge 型の 4-opt を適用する。時間計算量・追加空間計算量は $O(n)$。

### `apply_double_bridge(tour, a, b, c, d)`

局所最適からの脱出に使う double-bridge 操作。時間計算量・追加空間計算量は $O(n)$。

Segment Reversal、Segment Relocation / Node Insertion、Swap は、既存の `local_moves.hpp` の `reverse_segment`、`relocate_segment` / `relocate_vertex`、`swap_vertices` が提供する。`apply_or_opt` の `length = 1, 2, 3` が Or-opt-1/2/3 に対応する。

## Ejection API

### `EjectionMove`

移動元の経路と位置、移動先の経路と挿入位置を表す。

### `apply_ejection_chain(routes, moves)`

`moves` を順に実行する ejection chain。全経路の頂点数の合計を $n$、操作数を $q$ とすると、`vector` 内の移動を含む最悪時間計算量は $O(qn)$、追加空間計算量は $O(1)$。

### `EjectionPool<State, Score, Compare>`

容量付きの候補集合で、評価の良い上位 `capacity` 個を保持する。容量を $p$ として、`push` は $O(\log p)$、`size` と `empty` は $O(1)$、`sorted` は $O(p \log p)$。

## `path_relinking(current, target, maximum_steps = SIZE_MAX)`

2 つの順列の間を、要素を正しい位置へ動かす swap で結ぶ解の列を返す。返す解の列を含めて、時間計算量・追加空間計算量は $O(n^2)$。解の列を除く作業領域は $O(n)$。

## Lin–Kernighan 系 API

### `lin_kernighan_tour(distance, tour, maximum_passes = 100)`

すべての 2-opt 候補から最良の改善を選んで適用することを繰り返す、決定的な局所探索の基礎版。パス数の上限を $P$ として、1 パスは $O(n^2)$、合計は $O(Pn^2)$、追加空間計算量は $O(n)$。

### `lin_kernighan_helsgaun_tour(distance, tour, candidate_count = 20, maximum_passes = 100)`

各頂点の近傍候補（距離の近い `candidate_count` 頂点）を前計算し、探索範囲を絞る版。候補数を $c$ とすると、前計算は $O(n^2 \log c)$、改善は $O(Pnc)$、追加空間計算量は $O(nc)$。$n < 4$ または `candidate_count >= n` の場合は `lin_kernighan_tour` と同じ処理になる。

## 注意点

- 距離行列は正方で、巡回路は $[0, n)$ のハミルトン閉路を表す順列でなければならない。
- 2-opt の差分評価は、対称な距離と、負の差分を表現できる距離の型を前提とする。
- 重複するブロック、同じ経路どうしの `cross_exchange`、順列でない巡回路、容量 0 の `EjectionPool` など、値の組合せが不正な場合は `std::invalid_argument` を送出する。添字や区間の端が範囲外の場合は `std::out_of_range` を送出する。
