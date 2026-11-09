---
title: 3D Voronoi Diagram Internal Detail (三次元Voronoi図内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_set/voronoi_diagram_3d_detail.hpp
---

2 つの公開個別ヘッダが共有する、Delaunay 分割の双対の接続関係の構築を実装する。

## API

- `voronoi_diagram_3d_detail::from_delaunay(delaunay)`: 四面体・面・辺の接続関係から `VoronoiDiagram3` を構築する内部関数。
- `tetrahedron_circumcenter`: 正確な 2 進有理数（dyadic）による Cramer の公式から外心を復元する内部関数。
- その他の名前空間内の要素: リッジの頂点順序、無限半直線、セル面、ハッシュ索引のための内部型・関数。

## API別の時間計算量・空間計算量

Delaunay 四面体数を $T$、返す頂点・辺・リッジ・セル面と接続関係の総数を $K$ とする。ハッシュ表の操作を期待計算量で数えると、時間・追加領域ともに $O(T+K)$。
入力の Delaunay 分割の構築時間は、公開個別ヘッダ側で別に加わる。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。
アフィン次元が 3 未満では三次元の双対の接続関係を構築せず、母点と空のセルだけを返す。
利用側はデフォルト版またはランダム化版の個別ヘッダ、あるいは互換用の集約ヘッダを include すること。
