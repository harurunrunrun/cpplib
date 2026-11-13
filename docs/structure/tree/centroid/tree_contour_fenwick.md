---
title: Tree Contour Fenwick Index (重み付き木の距離帯 Fenwick 索引) [vertex_add_range_contour_sum_on_tree] [vertex_get_range_contour_add_on_tree]
documentation_of: ../../../../src/structure/tree/centroid/tree_contour_fenwick.hpp
---

重み付き木で、ある頂点からの距離が指定範囲（距離帯）に入る頂点全体に対する和の取得または一様加算を扱う。
重心分解の各重心とブランチごとに、距離を座標圧縮した Fenwick 木を持つ。

## API

```cpp
TreeContourFenwick<T, Distance, MAX_SIZE = -1, RangeAdd = false>(values, edges)
```

頂点の初期値 `values` と辺の列 `edges` から構築する。`edges` は `(u, v, 非負の距離)` を要素とする `vector<tuple<int, int, Distance>>` である。
`MAX_SIZE` を省略すると可変長版になる。

- `RangeAdd = false`: `point_add(v, x)` と `contour_sum(v, l, r)` を使える。後者は `v` からの距離が `[l, r)` の頂点の値の和を返す。
- `RangeAdd = true`: `contour_add(v, l, r, x)` と `point_get(v)` を使える。前者は `v` からの距離が `[l, r)` の全頂点に `x` を加える。
- `size()`: 頂点数を返す。

## 時間計算量

$N$ を頂点数とする。重心分解と距離の座標圧縮により、構築は $O(N\log^2 N)$、各更新・取得は $O(\log^2 N)$ である。
使用空間は $O(\texttt{MAX_SIZE}\log\texttt{MAX_SIZE}+N\log N)$（可変長版は $O(N\log N)$）である。

## 注意点

- 2 つのモードは `RangeAdd` で切り替え、モードに対応しない操作は `requires` によりコンパイル時に呼び出せない。
- 重み 0 の辺、負の距離帯端点、空の距離帯（`l == r`）を許す。距離帯は `l <= r` を要求し、`l > r` なら `invalid_argument` を送出する。
- 距離の加算と、端点から距離を引いた値が `Distance` に収まること。
- 木の検査と距離のオーバーフロー検査は内部の重心距離索引（`CentroidDistanceIndex`）が行う。
- `T` は `+=`、`-=`、単項 `-` に対応する必要がある。
