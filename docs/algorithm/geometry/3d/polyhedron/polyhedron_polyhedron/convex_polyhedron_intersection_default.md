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

$H=F_1+F_2$、出力sizeを $K$ とする。exact空判定は期待
$O(V_1+V_2+H)$、最悪 $O(V_1+V_2+H^3)$ 時間。strict interiorがある
full-dimensionalな通常経路は期待 $O(V_1+V_2+H\log H+K)$ 時間、
$O(V_1+V_2+H+K)$ 追加領域。低次元接触または双対の退化では、三平面の
全組合せを列挙せず、直ちに次の候補点fallbackへ移る。

候補点fallbackでは候補点数を $C$、全辺queryが訪問するface-AABB node数の
合計を $K_S$ とする。両入力が3次元ならquery階層を構築し、列挙は
$O(N_1\log N_1+N_2\log N_2+V_1\log V_2+V_2\log V_1+K_S)$ 時間。
$K_S$ は通常、交差候補面だけに近いが、AABBが重なる最悪配置では
$O(E_1F_2+E_2F_1)$ である。低次元clipperは従来どおり
$O(V_1F_2+V_2F_1+E_1F_2+E_2F_1)$ 時間となる。
候補点の凸包構築は決定的に最悪 $O(C\log C+K)$ 時間、$O(C+K)$ 領域。
query階層を含むfallback全体の追加領域は $O(N_1+N_2+C+K)$。

## 注意点

入力は各頂点列の凸包を表す必要がある。各下位APIの非有限座標・不正な面添字に対する例外がそのまま送出される。
半空間経路が退化した双対配置を構成できない場合は、例外を外へ出さず
候補点経路へ切り替える。
