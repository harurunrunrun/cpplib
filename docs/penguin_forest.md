---
title: Penguin Forest (動的森の頂点和) [OTOCI]
documentation_of: ../src/structure/tree/penguin_forest.hpp
---

頂点値を持つ動的な森について、辺追加・頂点代入・パス和を扱う。

## API

### `PenguinForest<MAX_SIZE>(penguins)`

初期頂点値から孤立頂点の森を作る。時間計算量は $O(n)$。

### `size()`

頂点数を返す。時間計算量は $O(1)$。

### `bridge(left, right)`

異なる木なら辺を追加して `true`、既に連結なら `false` を返す。時間計算量は償却 $O(\log n)$。

### `set_penguins(vertex, count)`

頂点値を代入する。時間計算量は償却 $O(\log n)$。

### `excursion_sum(left, right)`

連結なら両端を含むパス和、非連結なら `nullopt` を返す。時間計算量は償却 $O(\log n)$。

## 注意点

- `0 <= n <= MAX_SIZE` と範囲内の頂点番号が必要である。
- 多重辺とcycleは追加されない。
