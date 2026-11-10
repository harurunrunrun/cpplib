---
title: 3D Voronoi Diagram with Default Seed (固定シードによる三次元Voronoi図)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/voronoi_diagram_3d_default.hpp
---

固定シードで構築した Delaunay 四面体分割（`delaunay_tetrahedralization_3d`）の双対として、三次元 Voronoi 図を構築する。

## API

- `voronoi_diagram_3d(points)`: 重複したサイトを除き、有限頂点・セル・リッジ（Voronoi 面）・辺・無限半直線とそれらの接続関係を持つ `VoronoiDiagram3` を返す。

## API別の時間計算量・空間計算量

サイト数を $N$、Delaunay 構築の手間を $D, C$（`delaunay_tetrahedralization_3d` の $K, C$ に相当）、出力と接続関係の総数を $K$ とする。ハッシュ操作を期待 $O(1)$ とみなすと、時間 $O(N \log N + D + C + K)$、追加空間 $O(N + D + K)$。三次元の出力サイズは最悪 $O(N^2)$。

## 注意点

- アフィン次元が 3 未満なら、サイトと空のセルレコードだけを返す。
- 固定シードに対して敵対的に作られた入力では、Delaunay 構築の期待計算量の上界は保証しない。
- 外心が有限範囲を超える場合は `std::overflow_error` を送出する。座標が非有限の場合は `std::invalid_argument` を送出する。
- 辺・リッジの格納順は仕様に含まれないため、添字フィールドを通じて参照すること。
