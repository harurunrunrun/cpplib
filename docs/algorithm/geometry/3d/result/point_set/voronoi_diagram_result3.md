---
title: VoronoiDiagram3 (三次元Voronoi図結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/voronoi_diagram_result3.hpp
---

三次元 Voronoi 図を、Delaunay 双対としての位相と実際の幾何の両方で保持する型群を提供する。

## API

- `VoronoiRay3{origin, direction}`: 非有界な Voronoi 辺を表す半直線。有限の始点 `origin` と外向きの単位ベクトル `direction` を持つ。
- `VoronoiCell3`: サイト `site`、有限頂点の添字 `finite_vertices`、隣接サイト `neighbors`、対応するリッジの添字 `ridge_indices`、各隣接サイトとの垂直二等分半空間 `boundary_halfspaces`、非有界かどうか `unbounded` を持つ。有界なセルでは、構成できる場合に `bounded_polyhedron`（`std::optional<ConvexPolyhedron3>`）も持つ。
- `VoronoiRidge3`: 2 サイト `sites` 間の二次元 Voronoi 面。`finite_vertices` は有界な面では巡回順、非有界な面では経路順に並ぶ。境界の辺の添字 `edge_indices` と、非有界な面の両端の外向き半直線 `unbounded_rays` を持つ。
- `VoronoiEdge3`: 3 サイト `sites` から等距離な一次元要素。有界なら `segment`、非有界なら始点と方向からなる `ray` を持つ。共球面の退化では、1 点だけからなる有界な辺もあり得る。
- `VoronoiDiagram3::affine_dimension`: サイト集合のアフィン次元（空集合は $-1$）。
- `VoronoiDiagram3::sites`: 完全一致する重複を除去したサイト。
- `finite_vertices`, `cells`, `ridges`, `edges`: 共通の添字系で参照し合う双対幾何。

## API別の時間計算量・空間計算量

メンバへのアクセスは $O(1)$、添字列の走査はその長さに比例する。サイト数を $N$、Delaunay 四面体の数を $T$ とすると、全格納領域は $O(N + T)$。

## 注意点

- 共球面な複数の四面体の外心が一致する場合は、1 つの `finite_vertices` 要素に統合する。
- 非有界な要素は無限遠の座標ではなく、単位方向ベクトル付きの半直線で表す。
- `boundary_halfspaces` の各平面は、セルのサイト側が $\mathbf{n} \cdot (\mathbf{x} - \mathbf{p}) \le 0$（$\mathbf{n}$ は `normal`、$\mathbf{p}$ は `point`）となる向きで格納する。
