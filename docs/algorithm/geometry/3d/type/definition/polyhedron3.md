---
title: Polyhedron3 (三次元一般多面体)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/polyhedron3.hpp
---

凸性を仮定しない三次元多面体を、頂点列と向き付きの三角形面の列で表す。

## API

- `Polyhedron3::vertices`: 頂点座標の列（`std::vector<Point3>`）。
- `Polyhedron3::faces`: 各三角形面の 3 頂点のインデックス（`std::vector<std::array<std::size_t, 3>>`）。立体の外側から見て反時計回りに並べる。

## API別の時間計算量・空間計算量

- 各メンバの参照は時間・追加領域ともに $O(1)$。全要素の列挙は、頂点数を $V$、面数を $F$ として $O(V + F)$。

## 注意点

体積・包含判定などの立体操作では、面が閉じた二次元多様体をなし、向きが一貫していることを前提とする。凸性は要求しない。型自体はこれらを検査しない。
