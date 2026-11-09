---
title: Polygon Offset (多角形のオフセット)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set_scalar/polygon_offset.hpp
---

単純多角形からの符号付きユークリッド距離の等高線（レベルセット）を、線分と、丸め結合（round join）による解析的な円弧からなる複数の閉じた境界として構築する。凹多角形のほか、オフセットに伴う分裂・消滅・穴の発生も扱う。

## API

- `PolygonOffsetEdgeKind`: 境界片の種類で、`segment`（線分）または `circular_arc`（円弧）。
- `PolygonOffsetSourceKind`: 境界片の生成元の種類で、元の多角形の `edge`（辺）または `vertex`（頂点）。
- `PolygonOffsetEdge`:
  - `kind`, `first`, `second`: 曲線の種類と、向き付きの始点・終点。
  - 円弧の場合は `center`, `radius`, `first_angle`, `second_angle`, `counterclockwise` も有効である。
  - `source_kind`, `source_index`: 線分片なら元の辺、円弧片なら元の頂点を表す。添字は後述の正規化後の多角形に対するものである。
- `PolygonOffsetResult`: `edges` は解析的な境界片の列、`contours` は向き付きの境界片の番号（`edges` の添字）を並べた閉路の列。
- `PolygonOffsetResult polygon_offset(std::vector<Point> polygon, long double distance)`: `distance` が正なら外向き、負なら内向きにオフセットし、$0$ なら元の境界を返す。

## 時間計算量

頂点数を $N$、残る境界片の個数を $K = O(N^2)$ とする。交点候補の生成と距離判定に $O(N^3)$、端点のクラスタリングと接線角の順による閉路の連結に $O(K \log K)$ かかる。全体で時間 $O(N^3)$、空間 $O(N^2)$。

## 注意点

- 入力は自己交差しない非退化な単純多角形で、座標と距離は有限値でなければならない（単純性は検査しない）。
- 入力は最初に正規化される。末尾に繰り返された先頭頂点と連続する重複頂点を除き、時計回りなら反時計回りに反転する。`source_index` と `distance = 0` のときの結果はこの正規化後の頂点列に基づく。
- 正規化後の頂点数が $3$ 未満、`distance` が非有限、または座標が非有限なら `std::invalid_argument` を送出する。
- マイター結合のオフセットではない。円弧を固定角度で分割せず、解析的に保持する。
