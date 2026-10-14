---
title: Dynamic Nearest Marked Vertex (動的マーク頂点最近傍)
documentation_of: ../../../../src/structure/tree/centroid/dynamic_nearest_marked_vertex.hpp
---

非負重み付き木上でマークされた頂点集合を更新し、指定頂点から最も近いマーク頂点までの距離を求める。
内部では重心分解を用いる。

```cpp
DynamicNearestMarkedVertex<long long, 200000> nearest(n);
nearest.add_edge(u, v, weight);
nearest.build();
nearest.mark(v);
auto distance = nearest.nearest_distance(u);
```

## API

### `DynamicNearestMarkedVertex<Distance, MAX_SIZE>(n = MAX_SIZE)`

頂点数 `n` の構造を作る。頂点番号は $[0,n)$ とする。
`Distance` は `bool` 以外の整数型とする。

- 時間計算量: $O(\mathrm{MAX\_SIZE}\log(\mathrm{MAX\_SIZE}+1))$
- 空間計算量: $O(\mathrm{MAX\_SIZE}\log(\mathrm{MAX\_SIZE}+1))$

### `size()`

頂点数を返す。

- 時間計算量: $O(1)$

### `add_edge(u, v, weight)`

重み `weight` の無向辺を追加する。重みは非負でなければならない。
`build` より前にちょうど $n-1$ 本追加する。

- 時間計算量: 償却 $O(1)$

### `build(initially_marked = false)`

追加した木を重心分解し、更新とクエリを可能にする。
`initially_marked` が `true` なら全頂点を、`false` ならどの頂点もマークされていない状態にする。

- 時間計算量: `initially_marked == false` なら $O(n\log(n+1))$、`true` なら $O(n\log^2(n+1))$
- 空間計算量: $O(\mathrm{MAX\_SIZE}\log(\mathrm{MAX\_SIZE}+1))$

### `marked_count()`

現在マークされている頂点数を返す。

- 時間計算量: $O(1)$

### `is_marked(v)`

頂点 `v` がマークされているかを返す。

- 時間計算量: $O(1)$

### `set_marked(v, value)`

頂点 `v` の状態を `value` にする。同じ状態を指定した場合は何もしない。

- 時間計算量: $O(\log^2(n+1))$

### `mark(v)` / `unmark(v)`

頂点 `v` をマークする、またはマークを外す。既に同じ状態なら何もしない。

- 時間計算量: $O(\log^2(n+1))$

### `toggle(v)`

頂点 `v` のマーク状態を反転する。

- 時間計算量: $O(\log^2(n+1))$

### `nearest_distance(v)`

頂点 `v` からマーク頂点までの最小距離を返す。`v` 自身がマークされていれば `Distance{}` を返す。
マーク頂点が存在しなければ `std::nullopt` を返す。

- 時間計算量: $O(\log(n+1))$

## API別の追加空間計算量

constructorはobject本体へ O(MAX_SIZE log MAX_SIZE) を確保する。size、marked_count、is_marked、nearest_distanceは O(1)。add_edgeは本体へ O(1) の辺領域を追加する。buildは作業vector O(n) を使い、initially_markedならmultiset nodeを合計 O(n log n) 保存する。set_marked、mark、unmark、toggleは状態が変わる場合に本体のmultiset nodeを O(log n) 個増減し、一時領域は O(1)。

## 注意点

`n` が $[0,\mathrm{MAX\_SIZE}]$ の外、端点が範囲外、辺重みが負、入力が木でない、`build` 前にクエリまたは更新を行った場合は `runtime_error` を送出する。
距離の加算が `Distance` の表現範囲を超えると `overflow_error` を送出する。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = DynamicNearestMarkedVertex<Distance>;
using Fixed = DynamicNearestMarkedVertex<Distance, N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
