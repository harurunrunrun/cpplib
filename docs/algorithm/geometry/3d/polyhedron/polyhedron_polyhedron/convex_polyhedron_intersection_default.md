---
title: Convex Polyhedron Intersection by Candidate Points (候補点列挙による凸多面体共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/convex_polyhedron_intersection_default.hpp
---

相手に含まれる入力頂点と、相手でclipした入力辺の端点を列挙し、その凸包を返す。

## API

- `convex_polyhedron_intersection(first, second)`: 空集合や低次元接触を含む共通部分を `ConvexPolyhedron3` として返す。

## API別の時間計算量・空間計算量

各入力の頂点・辺・三角形面数を $V_i,E_i,F_i$、候補点数を $C$、出力sizeを $K$ とする。候補列挙は $O(V_1F_2+V_2F_1+E_1F_2+E_2F_1)$ 時間。凸包構築は固定seedで最悪 $O(C^2\log C+K)$ 時間・$O(C^2+K)$ 追加領域、非敵対的入力では期待 $O(C\log C+K)$ 時間・$O(C+K)$ 領域。

## 注意点

入力は各頂点列の凸包を表す必要がある。各下位APIの非有限座標・不正な面添字に対する例外がそのまま送出される。
