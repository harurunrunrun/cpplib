---
title: Offline Road Disconnection (オフライン道路切断) [NITTROAD]
documentation_of: ../src/structure/dsu/offline_road_disconnection.hpp
---

木辺が一度ずつ削除される列に対し、非連結な頂点対数をまとめて求める。

## API

### `OfflineRoadDisconnection<MAX_SIZE>(n)`

頂点数 `n` の空のquery列を作る。時間計算量は $O(n)$。

### `add_road(left, right)`

木辺を追加し、その0-indexed番号を返す。時間計算量は償却 $O(1)$。

### `remove_road(road)`

指定辺の削除をquery列へ追加する。時間計算量は償却 $O(1)$。

### `ask()`

非連結頂点対数の取得queryを追加する。時間計算量は償却 $O(1)$。

### `solve()`

取得query順の回答列を返す。query数を $Q$ として時間計算量は $O((n+Q)\alpha(n))$。

## 注意点

- `0 <= n <= MAX_SIZE`、道路数はちょうど `n-1` が必要である。
- 同じ道路を二度削除できない。不正な番号や本数では `runtime_error`。
- 入力道路は木であることを呼び出し側が保証する。
