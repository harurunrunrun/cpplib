---
title: Convex Polyhedron Plane Section (凸多面体の平面断面)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polygon/polyhedron_plane/convex_polyhedron_plane_section.hpp
---

## API

- `convex_polyhedron_plane_section(polyhedron, plane)`: 凸多面体と平面の共通部分の頂点を、平面上の凸順序（法線方向から見て反時計回り）で `Polygon3` として返す。共通部分が空・1 点・線分の場合も、それぞれ $0$・$1$・$2$ 頂点の列で表す。

## API 別の時間計算量・空間計算量

辺数を $E$、交点数を $K$ として、時間 $O(E \log E + K \log K)$、領域 $O(E + K)$。辺の列挙（`convex_polyhedron_edges`）は `std::map` を使うため $O(E \log E)$ かかる。

## 注意点

- 零法線の平面、非有限な入力には `std::invalid_argument` を送出する。
- 同一平面上にある三角形分割の対角線は、辺の列挙時に除外する。
- 面を持たない `affine_dimension` が $1$ 以下の多面体（1 点・線分）は、`vertices` の先頭と末尾を結ぶ線分として扱う。
- 頂点の側判定と凸順序の判定は、法線を正規化せずに入力の `long double` を exact dyadic 数とみなす厳密演算（側判定は相対許容誤差付き）で行う。このため、巨大な平行移動に比べて非常に薄い断面も保持する。辺との交点は `line_plane_intersection` で求める。
