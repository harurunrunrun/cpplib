---
title: Marked Tree Diameter Queries (マーク頂点直径クエリ) [QTREE4]
documentation_of: ../../../../src/structure/tree/centroid/marked_tree_diameter_queries.hpp
---

重み付き木でmark頂点集合を動的に変更し、その直径を返す。

## API

### `MarkedTreeDiameterQueries<Weight, MAX_SIZE>(n)`

空の木を作る。時間計算量は $O(n)$。

### `add_edge(left, right, weight)`

重み付き辺を追加する。時間計算量は償却 $O(1)$。

### `build(initially_marked = true)`

索引を構築し、全頂点の初期mark状態を指定する。時間計算量は $O(n\log n)$。

### `toggle(vertex)`

markを反転する。時間計算量は $O(\log^2 n)$。

### `diameter()`

mark頂点間距離の最大値を返す。markがなければ `nullopt`。時間計算量は $O(1)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、辺集合は重み付き木でなければならない。
- `build` 前の操作、負辺、範囲外頂点では `runtime_error`。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = MarkedTreeDiameterQueries<Weight>;
using Fixed = MarkedTreeDiameterQueries<Weight, N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
