---
title: DelaunayTetrahedralization3 (三次元Delaunay四面体分割結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/delaunay_tetrahedralization_result3.hpp
---

三次元 Delaunay 四面体分割の頂点と四面体を保持する結果型 `DelaunayTetrahedralization3` を提供する。

## API

- `affine_dimension`（`int`、デフォルト値 $-1$）: 入力点集合のアフィン次元。空集合は $-1$、一点・共線・共面・三次元をそれぞれ $0, 1, 2, 3$ とする。
- `vertices`（`std::vector<Point3>`）: 完全一致する重複を除いた点を辞書順に保持する。
- `tetrahedra`（`std::vector<std::array<std::size_t, 4>>`）: `vertices` の添字 4 個からなる正の向きの四面体を保持する。

## API別の時間計算量・空間計算量

各メンバへのアクセスは時間・追加空間ともに $O(1)$。頂点数を $N$、四面体数を $T$ とすると、格納領域は $O(N + T)$。

## 注意点

- `affine_dimension < 3` のとき `tetrahedra` は空になる。
- 保証するのは、4 頂点をこの順に並べたときの `orient3d` が正になることであり、各面の向きについては保証しない。
