---
title: VoronoiDiagram3 (三次元Voronoi図結果)
documentation_of: ../src/algorithm/geometry/3d/shape/voronoi_diagram_result3.hpp
---

三次元Voronoi図をDelaunay双対の位相と実際の幾何として保持する型群。

## API

- `VoronoiRay3`: 無限Voronoi辺の有限 `origin` と外向きunit `direction`。
- `VoronoiCell3`: site、有限頂点、neighbor、対応ridge添字、各neighborとの垂直
  二等分halfspaceを持つ。有限cellでは構成可能な場合に `bounded_polyhedron` も持つ。
- `VoronoiRidge3`: 2 site間の二次元Voronoi面。`finite_vertices` は有限面では循環順、
  非有界面ではpath順で、境界edge添字と両端の外向きrayを持つ。
- `VoronoiEdge3`: 3 siteから等距離な一次元要素。有限なら `segment`、非有界なら
  origin+directionの `ray` を持つ。共球面退化では一点だけの有限edgeもあり得る。
- `VoronoiDiagram3::sites`: 完全一致重複を除去したsite。
- `finite_vertices`, `cells`, `ridges`, `edges`: 共通添字系の双対幾何。

## API別の時間計算量・空間計算量

member参照は $O(1)$、添字列の走査はその長さに比例する。全格納領域はDelaunay
四面体数を $T$ として $O(N+T)$。

## 注意点

- 共球面な複数四面体の一致外心は一つの `finite_vertices` 要素へ統合する。
- 非有界要素を無限遠座標で表現せず、unit direction付きrayで表す。
- `boundary_halfspaces` はcellのsite側を $normal\cdot(x-point)\leq0$ とする。
