---
title: Convex Polyhedron Intersection (凸多面体共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/convex_polyhedron_intersection_default.hpp
---

三次元入力では両入力の面半空間をまとめ、極双対によって共通部分を構築する。
退化した双対配置または低次元入力では、頂点包含と辺clipで候補点を列挙し、
その凸包として構築する。

## API

- `convex_polyhedron_intersection(first, second)`: 空集合や低次元接触を含む共通部分を `ConvexPolyhedron3` として返す。

## API別の時間計算量・空間計算量

$H=F_1+F_2$、出力sizeを $K$ とする。full-dimensionalな通常経路は
期待 $O(V_1+V_2+H\log H+K)$ 時間、$O(V_1+V_2+H+K)$ 追加領域。
空集合・低次元接触・双対の退化を処理する半空間経路は最悪
$O(V_1+V_2+H^4+K)$ 時間、$O(V_1+V_2+H+K)$ 追加領域。

候補点fallbackでは候補点数を $C$ とする。列挙は
$O(V_1F_2+V_2F_1+E_1F_2+E_2F_1)$ 時間。凸包構築は固定seedで最悪
$O(C^2\log C+K)$ 時間・$O(C^2+K)$ 追加領域、非敵対的入力では
期待 $O(C\log C+K)$ 時間・$O(C+K)$ 追加領域。

## 注意点

入力は各頂点列の凸包を表す必要がある。各下位APIの非有限座標・不正な面添字に対する例外がそのまま送出される。
半空間経路が退化した双対配置を構成できない場合は、例外を外へ出さず
候補点経路へ切り替える。
