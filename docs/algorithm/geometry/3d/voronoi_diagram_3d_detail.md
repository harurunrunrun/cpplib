---
title: 3D Voronoi Diagram Internal Detail (三次元Voronoi図内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/voronoi_diagram_3d_detail.hpp
---

二つの公開leafが共有するDelaunay双対incidence構築を保持する。

## API

- `voronoi_diagram_3d_detail::from_delaunay(delaunay)`: 四面体・面・辺の
  incidenceから `VoronoiDiagram3` を構築する内部関数。
- `tetrahedron_circumcenter`: exact dyadic Cramer式から外心を復元する内部関数。
- その他のnamespace内要素: ridge順序、無限ray、cell面、hash索引用の内部型・関数。

## API別の時間計算量・空間計算量

Delaunay四面体数を $T$、返す頂点・edge・ridge・cell面とincidence総数を $K$
とする。期待hash計算量の下で時間・追加領域ともに $O(T+K)$。
入力Delaunayの構築時間は公開leaf側に加わる。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。
アフィン次元3未満では三次元双対incidenceを構築しない。
利用側はdefaultまたはrandomized leaf、あるいは互換aggregatorをincludeする。
