---
title: Plug DP Framework (plug DPフレームワーク)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/profile_grid/plug_dp.hpp
---

格子の境界線（frontier）上の各位置に連結成分のラベルをもつ、プラグ DP（plug DP）の共通の状態クラスと遷移エンジン。名前空間は `dynamic_programming` である。

## `PlugDpState`

### `PlugDpState(frontier_size)` / `PlugDpState(plugs)`

長さ `frontier_size` の空の境界、または指定したラベル列（構築時に正規化する）を構築する。構築の時間・空間計算量は $O(W)$（$W$ は境界の長さ）。

### `size()` / `operator[](index)` / `set(index, label)` / `erase(index)` / `plugs()`

境界を参照・更新する。`erase(index)` はその位置のラベルを $0$（空き）にする。`plugs()` はラベル列全体への参照を返す。いずれも $O(1)$ 時間で、添字は `at` で検査する。

### `connect(first, second)`

2 つのラベルの連結成分を併合し、正規化する。時間計算量 $O(W)$、追加の空間計算量 $O(W)$。

### `canonicalize()`

先頭から現れる順にラベルを $1,2,\ldots$ に付け直す。時間・空間計算量は $O(W + L)$（$L$ はラベルの最大値。正規化済みなら $L \le W$）。

## `PlugDpStateHash`

正規化した状態のハッシュ値を計算する。時間計算量 $O(W)$。

## `plug_dp_grid<Count>(row_count, column_count, initial_state, transition, accept)`

行優先の順に各マス・各状態について `transition(row, column, state, emit)` を呼ぶ。`emit(next_state, multiplier)` で、正規化した `next_state` に現在の場合の数の `multiplier` 倍を加算する。全マスの処理後、`accept(state)` が真となる状態の場合の数の合計を返す。状態は `std::unordered_map` で管理する。

到達する状態数を $S$、1 状態からの遷移数を $D$、境界の幅を $W$、格子の高さと幅を $H$, $C$ とすると、正規化を含む時間計算量は期待 $O(HCSDW)$、空間計算量は $O(SW)$ である。

## 注意点

- ラベル $0$ は空きを表す。
- `connect` に $0$ を渡した場合は `std::invalid_argument`、範囲外の添字には `std::out_of_range` を送出する。
