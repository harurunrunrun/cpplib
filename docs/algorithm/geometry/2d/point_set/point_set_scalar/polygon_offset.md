---
title: Polygon Offset (多角形オフセット)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set_scalar/polygon_offset.hpp
---

単純多角形からの符号付き Euclid 距離 level set を、round join の線分と解析的円弧からなる複数の境界閉路として構築する。凹多角形、分裂、消滅、穴を扱う。

## API

- `PolygonOffsetEdgeKind` は `segment` または `circular_arc`。
- `PolygonOffsetSourceKind` は元の多角形 `edge` または `vertex`。
- `PolygonOffsetEdge::kind`, `first`, `second` は曲線種類と向き付き端点。円弧では `center`, `radius`, `first_angle`, `second_angle`, `counterclockwise` も有効。
- `PolygonOffsetEdge::source_kind`, `source_index` は線分片なら元辺、円弧片なら元頂点を示す。
- `PolygonOffsetResult::edges` は解析的境界片、`contours` は向き付き edge 番号による閉路列。
- `polygon_offset(polygon, distance)` は正で外向き、負で内向き、0で元境界を返す。

## 時間計算量

頂点数を $N$、残る境界片数を $K=O(N^2)$ とする。候補交点生成と距離判定は $O(N^3)$、
endpoint clustering と接線角順 stitch は $O(K\log K)$。全体は $O(N^3)$ 時間、$O(N^2)$ 空間。

## 注意点

入力は自己交差しない非退化単純多角形で、座標と距離は有限値でなければならない。
miter offset ではない。固定角度分割を行わず円弧を解析的に保持する。
