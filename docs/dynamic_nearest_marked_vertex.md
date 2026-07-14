---
title: Dynamic Nearest Marked Vertex (動的マーク頂点最近傍) [QTREE5]
documentation_of: ../src/structure/tree/dynamic_nearest_marked_vertex.hpp
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
