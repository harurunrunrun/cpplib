---
title: Offline Tree Path Distinct (木上パス異なる値数) [COT2]
documentation_of: ../src/algorithm/tree/offline_tree_path_distinct.hpp
---

静的な頂点値付き木について、複数パスの異なる値数をofflineで返す。

## API

### `OfflineTreePathDistinct<Value, MAX_SIZE>(vertex_values)`

頂点値を保持する。時間計算量は $O(n)$。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は償却 $O(1)$。

### `build(root = 0)`

Euler tourと値圧縮を構築する。時間計算量は $O(n\log n)$。

### `add_query(left, right)`

パスqueryを追加して番号を返す。時間計算量は償却 $O(1)$。

### `solve()`

追加順の異なる値数を返す。query数を $Q$ として時間計算量は $O((n+Q)\sqrt n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下、辺集合は連結な木でなければならない。
- `build` 後にqueryを追加し、すべて追加してから `solve` を呼ぶ。
- 範囲外頂点や不正な木では `runtime_error`。
