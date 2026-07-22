---
title: 2D Circle-Line Intersection Test (二次元円と直線の交差判定)
documentation_of: ../src/algorithm/geometry/2d/predicate/intersect_circle_line.hpp
---

## API

- `intersect_circle_line(circle, line)`: 円周と無限直線が共通点を持つか判定する。

## API別の時間計算量・空間計算量

- `intersect_circle_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

接する場合も真。負半径または退化直線には `std::invalid_argument` を送出する。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- lineの方向vector長がGEOMETRY_EPS以下ならstd::invalid_argumentを送出する。
