---
title: Convex Polyhedron Centroid (凸多面体の重心)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/polyhedron/convex_polyhedron_centroid.hpp
---

## API

- `convex_polyhedron_centroid(polyhedron)`: `affine_dimension` に応じて、点・線分・平面凸多角形・立体凸多面体の重心を返す。
  - $0$ 次元以下: 先頭頂点
  - $1$ 次元: 先頭頂点と末尾頂点の中点
  - $2$ 次元: 三角形面を面積で重み付けした重心
  - $3$ 次元: 先頭頂点を頂点とする有向四面体を体積で重み付けした重心

## API別の時間計算量・空間計算量

- 頂点数を $V$、面数を $F$ として時間 $O(V+F)$、追加領域 $O(V)$。

## 注意点

- 1 次元では先頭頂点と末尾頂点を線分の両端とみなす。
- 立体では、閉じていて向きが一貫した三角形面を前提とする。
- 頂点が空、または非有限の頂点があれば `std::invalid_argument` を送出する。
- 面の頂点添字が範囲外なら `std::out_of_range` を送出する。
- 2 次元で面積の合計が $0$、3 次元で体積の合計が $0$ の場合は `std::domain_error` を送出する。
- 全頂点が原点にある場合は先頭頂点を返す。結果がオーバーフローした場合は `std::overflow_error` を送出する。
