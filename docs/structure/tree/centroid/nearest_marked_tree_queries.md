---
title: Nearest Marked Tree Queries (動的最近傍マーク頂点) [QTREE5]
documentation_of: ../../../../src/structure/tree/centroid/nearest_marked_tree_queries.hpp
---

重み付き木上のmark集合を反転し、指定頂点から最近いmarkまでの距離を返す。

## API

### `NearestMarkedTreeQueries<Weight, MAX_SIZE>(n)`

全頂点未markの空の木を作る。時間計算量は $O(n)$。

### `add_edge(left, right, weight = Weight{1})`

重み付き辺を追加する。時間計算量は償却 $O(1)$。

### `build()`

centroid分解索引を構築する。時間計算量は $O(n\log n)$。

### `toggle(vertex)`

markを反転する。時間計算量は $O(\log^2 n)$。

### `nearest_distance(vertex)`

最近のmarkまでの距離を返し、markがなければ `nullopt`。時間計算量は $O(\log^2 n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下、辺集合は非負重みの木でなければならない。
- `build` 前の操作や範囲外頂点では `runtime_error`。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = NearestMarkedTreeQueries<Weight>;
using Fixed = NearestMarkedTreeQueries<Weight, N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
