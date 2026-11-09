---
title: Exact Convex Polyhedron Intersection Internal Detail (厳密凸多面体交差判定内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/polyhedron/convex_polyhedron_intersects_detail.hpp
---

厳密な乱択判定と厳密な分離軸判定の公開個別ヘッダが共有する、正確な演算・検証・探索の実装である。

## API

- `convex_polyhedron_intersects_detail` 内の型と関数: 有限な座標を 2 進有理数（dyadic）の整数表現へ変換し、半空間の実行可能性と分離軸を厳密に判定する内部要素。
- `independent_random_engine()`: シードを指定しない版で制約の順序を決めるための、`std::random_device` で初期化した `std::mt19937_64` を返す。

## API別の時間計算量・空間計算量

2 つの多面体の頂点数を $V_1, V_2$、面数を $F_1, F_2$、辺数を $E_1, E_2$ とする。

- 乱択の実行可能性判定: $H=F_1+F_2$ として、期待時間・領域 $O(V_1+V_2+H)$、最悪時間 $O(V_1+V_2+H^3)$。
- 分離軸判定: $A=F_1+F_2+E_1E_2$ として、時間 $O((F_1+F_2)\log(E_1+E_2)+A(V_1+V_2))$、領域 $O(V_1+V_2+E_1+E_2)$。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。多倍長整数演算のビット計算量は、座標の指数の幅と多倍長整数の語数に依存する。
