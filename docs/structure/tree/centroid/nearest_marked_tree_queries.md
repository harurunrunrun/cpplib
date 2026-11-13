---
title: Nearest Marked Tree Queries (動的最近傍マーク頂点) [QTREE5]
documentation_of: ../../../../src/structure/tree/centroid/nearest_marked_tree_queries.hpp
---

重み付き木上でマークされた頂点の集合を反転操作で変更し、指定した頂点から最も近いマーク頂点までの距離を返す。
`DynamicNearestMarkedVertex` の薄いラッパーである。

## API

### `NearestMarkedTreeQueries<Weight, MAX_SIZE>(n)`

全頂点が未マークで、辺のない頂点数 `n` の木を作る。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。
時間計算量は固定長版で $O(\texttt{MAX_SIZE}\log \texttt{MAX_SIZE})$、可変長版で $O(n)$ である。

### `add_edge(left, right, weight = Weight{1})`

重み付きの無向辺を追加する。時間計算量は償却 $O(1)$ である。

### `build()`

重心分解の索引を構築する。全頂点は未マークで開始する。時間計算量は $O(n\log n)$ である。

### `toggle(vertex)`

マークを反転する。時間計算量は $O(\log^2 n)$ である。

### `nearest_distance(vertex)`

最も近いマーク頂点までの距離を返す。`vertex` 自身がマークされていれば `Weight{}` を返し、マークがなければ `nullopt` を返す。時間計算量は $O(\log n)$ である。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、辺集合は非負重みの木でなければならない。
- `build` 前の操作、範囲外の頂点、負の重み、木でない入力では `runtime_error` を送出する。
- 距離が `Weight` で表現できなければ `overflow_error` を送出する。
- 容量を指定した固定配列版（`NearestMarkedTreeQueries<Weight, N>`）と可変長版（`NearestMarkedTreeQueries<Weight>`）は同じヘッダで利用できる。
