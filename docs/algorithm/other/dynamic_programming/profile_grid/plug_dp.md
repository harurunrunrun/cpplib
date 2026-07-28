---
title: Plug DP Framework (plug DPフレームワーク)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/profile_grid/plug_dp.hpp
---

格子のfrontier上に連結成分labelを持つplug DPの共通状態と遷移エンジン。

## `PlugDpState`

### `PlugDpState(frontier_size)` / `PlugDpState(plugs)`

空frontierまたは指定label列を構築する。構築時間・空間計算量は $O(W)$。

### `size()` / `operator[](index)` / `set(index, label)` / `erase(index)`

frontierを参照・更新する。`size` は $O(1)$、添字操作は $O(1)$。

### `connect(first, second)`

2 labelの連結成分を併合する。時間計算量 $O(W)$、追加空間計算量 $O(W)$。

### `canonicalize()`

先頭出現順にlabelを $1,2,\ldots$ へ正規化する。時間・空間計算量は $O(W)$。

## `PlugDpStateHash`

正規化状態をhash化する。時間計算量 $O(W)$。

## `plug_dp_grid<Count>(row_count, column_count, initial_state, transition, accept)`

各cell・状態で `transition(row, column, state, emit)` を呼び、`emit(next_state, multiplier)` で遷移する。到達状態数を $S$、1状態からの遷移数を $D$、frontier幅を $W$ とすると、正規化を含む時間計算量 $O(HWSDW)$、空間計算量 $O(SW)$。

## 注意点

label 0は空きを表す。`connect` に0を渡した場合は `std::invalid_argument`、範囲外添字には `std::out_of_range` を送出する。
