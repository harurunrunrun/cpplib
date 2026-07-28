---
title: Straight Skeleton Result (直線骨格結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/straight_skeleton_result.hpp
---

直線骨格を頂点列、各頂点が生じたoffset時刻、頂点種別、無向辺列で格納する。

## API

- `StraightSkeletonVertexKind` は `contour_vertex`、`edge_event`、`split_event`、
  `peak_event`、`simultaneous_event` のいずれかである。異なる種類のeventが同一点・
  同時刻に重なった場合は `simultaneous_event` になる。
- `vertices[i]` は骨格頂点の座標である。入力頂点は同じ順序で先頭に格納される。
- `times[i]` はwavefrontが `vertices[i]` に到達した非負のoffset距離である。
  入力頂点では `0` である。
- `kinds[i]` は `vertices[i]` の種別である。
- `edges` は骨格の無向辺を頂点番号pairで格納する。自己辺と重複辺は含まない。

常に `vertices.size() == times.size() == kinds.size()` が成り立ち、`edges` の
両端は有効な頂点番号である。
単純多角形から構築した結果では `edges` は全頂点を結ぶ木を成す。

## 時間計算量

各member参照は $O(1)$、全走査は格納された頂点数と辺数に対して線形。

## 注意点

`times` は辺を伝う弧長ではなく、入力辺を内側へ平行移動した距離である。
