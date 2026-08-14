---
title: Polyhedron3 (三次元一般多面体)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/polyhedron3.hpp
---

凸性を仮定しない三次元多面体を、頂点列と有向三角形面列で表す。

## API

- `Polyhedron3::vertices`: 頂点座標列。
- `Polyhedron3::faces`: 各三角形面の3頂点index。立体の外側から見て反時計回りに並べる。

## API別の時間計算量・空間計算量

- 各member参照は時間・追加領域 $O(1)$。全要素の列挙は頂点数 $V$、面数 $F$ に対して $O(V+F)$。

## 注意点

体積・包含などの立体操作では、面が閉じた二次元多様体を作り、向きが一貫していることを前提とする。凸性は要求しない。
