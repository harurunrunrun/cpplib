---
title: 3D Plane Arrangement (三次元平面アレンジメント)
documentation_of: ../../../../../../src/algorithm/geometry/3d/plane/plane/plane_arrangement_3d.hpp
---

平面を順に挿入し、新しい平面上に既存の平面が作る直線アレンジメントの領域数を加えていくことで、三次元空間の領域数を求める。あわせて、相異なる交線と 3 平面の交点を列挙する。

## API

- `plane_arrangement_3d(planes)`: `PlaneArrangement3` を返す。`planes` には一致平面を除いて単位法線に正規化した平面、`region_count` には領域数、`intersection_lines` には相異なる交線、`intersection_points` には相異なる 3 平面の交点を入れる。一致する平面は入力順によらず一度だけ数え、平行な平面や多重の交点も領域数に正しく反映する。

## API別の時間計算量・空間計算量

平面数を $P$ とする。

| API・処理 | 時間計算量 | 空間計算量（出力を含む） |
| --- | --- | --- |
| 平面の正規化・一致平面の除去 | $O(P^2)$ | $O(P)$ |
| 領域数の計算 | $O(P^3\log P)$ | $O(P^2)$ |
| 3 平面の交点の列挙・重複除去 | $O(P^3\log P)$ | $O(P^3)$ |
| `plane_arrangement_3d` 全体 | $O(P^4+P^3\log P)=O(P^4)$ | $O(P^3)$ |

全体の $O(P^4)$ は、許容誤差付きで一致する交線を、既に残した交線との線形探索で重複除去する処理の最悪計算量による。3 平面の交点の重複除去は、`deduplicate_points_3d` の固定幅セル（絶対許容誤差 `GEOMETRY3D_EPS`）で行うため $O(P^3\log P)$ で済む。

## 注意点

- 法線は非零、点と法線の全座標は有限でなければならない。違反時は `std::invalid_argument` を送出する。
- 一致・平行・交点の同一性は許容誤差付きで判定する。
- 領域数が `std::uint64_t` の範囲を超える場合は `std::overflow_error` を送出する。
