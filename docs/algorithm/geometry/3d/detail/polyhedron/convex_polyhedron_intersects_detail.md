---
title: Exact Convex Polyhedron Intersection Internal Detail (厳密凸多面体交差判定内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/polyhedron/convex_polyhedron_intersects_detail.hpp
---

厳密乱択判定と厳密分離軸判定の公開leafで共有するexact arithmetic・検証・探索実装を保持する。

## API

- `convex_polyhedron_intersects_detail` 内の型と関数: 有限座標をdyadic整数へ変換し、半空間実行可能性と分離軸を厳密に判定する内部要素。
- `independent_random_engine()`: 非seed版の制約順を生成する内部乱数engineを返す。

## API別の時間計算量・空間計算量

乱択実行可能性判定は $H=F_1+F_2$ として期待 $O(V_1+V_2+H)$ 時間・領域、最悪 $O(V_1+V_2+H^3)$ 時間。分離軸判定は $A=F_1+F_2+E_1E_2$ として $O((F_1+F_2)\log(E_1+E_2)+A(V_1+V_2))$ 時間、$O(V_1+V_2+E_1+E_2)$ 領域。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。多倍長整数演算のビット計算量は座標の指数差とlimb数に依存する。
