---
title: 3D Circle and Circle Intersections (三次元円同士の交点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/circle_circle/circle_circle_intersections.hpp
---

## API

- `circle_circle_intersections(first, second)`: 空間内の 2 円周の相異なる交点を、$0$ 個から $2$ 個の `std::vector<Point3>` で返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、出力以外の追加領域 $O(1)$。

## 注意点

- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 2 円の平面が平行でない場合は、平面同士の交線と `first` の交点のうち `second` 上にあるものを返す。
- 平面が平行で同一平面上にない場合は空を返す。
- 正半径の 2 円が一致する場合は交点が無限個なので `std::domain_error` を送出する。一致する半径 $0$ の円同士では中心 1 点を返す。
- 中心の差分を正規化して計算し、中間値のオーバーフローを避ける。結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
