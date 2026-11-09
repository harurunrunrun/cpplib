---
title: 2D Circle-Line Intersection Test (二次元円と直線の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/circle_line/intersect_circle_line.hpp
---

## API

- `bool intersect_circle_line(circle, line)`: 円周と無限直線が共通点を持つか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 接する場合も `true` を返す。
- 円の中心座標と半径は有限値でなければならない。非有限値または負の半径には `std::invalid_argument` を送出する。
- `line` の方向ベクトルの長さが `GEOMETRY_EPS` 以下なら `std::invalid_argument` を送出する。
- 判定では、比較する 4 次の量（外積の二乗と、半径の二乗と方向ベクトルの長さの二乗の積）を、各項の大きさに `GEOMETRY_EPS` を掛けた相対許容誤差で比較する。中間値は有限な `long double` に収まる必要がある。
