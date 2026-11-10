---
title: Convex Polyhedron Intersection (凸多面体共通部分)
documentation_of: ../../../../../../src/algorithm/geometry/3d/polyhedron/polyhedron_polyhedron/convex_polyhedron_intersection_default.hpp
---

両入力が三次元なら、まず厳密な交差判定を行い、交差していれば両入力の面の半空間をまとめて極双対によって共通部分を構築する。双対配置が退化している場合や低次元の入力では、頂点の包含判定と辺のクリップで候補点を列挙し、その凸包として構築する。

## API

- `convex_polyhedron_intersection(first, second)`: 空集合や低次元の接触を含む共通部分を `ConvexPolyhedron3` として返す。

## API 別の時間計算量・空間計算量

$V_i, E_i, F_i$ を各入力の頂点・辺・面数、$N_i = V_i + E_i + F_i$、$H = F_1 + F_2$、出力サイズを $K$ とする。

- 厳密な空判定: 期待 $O(V_1 + V_2 + H)$、最悪 $O(V_1 + V_2 + H^3)$ 時間。
- 狭義内点を持つ full-dimensional な通常経路: 期待 $O(V_1 + V_2 + H \log H + K)$ 時間、$O(V_1 + V_2 + H + K)$ 追加領域。
- 低次元の接触または双対の退化では、三平面の全組合せは列挙せず、ただちに次の候補点フォールバックへ移る。

候補点フォールバックでは、候補点数を $C$、全辺のクエリが訪問する面 AABB ノード数の合計を $K_S$ とする。

- 両入力が三次元ならクエリ階層を構築し、列挙は $O(N_1 \log N_1 + N_2 \log N_2 + V_1 \log V_2 + V_2 \log V_1 + K_S)$ 時間。$K_S$ は通常、交差候補の面の数に近いが、AABB が重なる最悪の配置では $O(E_1 F_2 + E_2 F_1)$ である。
- 低次元の入力を含む場合のクリップは $O(V_1 F_2 + V_2 F_1 + E_1 F_2 + E_2 F_1)$ 時間。
- 候補点の凸包構築は決定的に最悪 $O(C \log C + K)$ 時間、$O(C + K)$ 領域。
- クエリ階層を含むフォールバック全体の追加領域は $O(N_1 + N_2 + C + K)$。

## 注意点

- 各入力は、その頂点列の凸包を表していなければならない。
- 下位の API が送出する非有限座標・不正な面添字に対する例外は、そのまま送出される。
- 半空間経路が退化した双対配置を構成できない場合（`std::overflow_error`・`std::domain_error`）は、例外を外へ出さずに候補点経路へ切り替える。
