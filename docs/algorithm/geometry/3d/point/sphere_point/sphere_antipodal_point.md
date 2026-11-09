---
title: Antipodal Point on 3D Sphere (三次元球面上の反対点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/sphere_point/sphere_antipodal_point.hpp
---

## API

- `sphere_antipodal_point(sphere, point)`: 球面上の点 `point` に対し、球の中心に関して反対側にある球面上の点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `Sphere3` は中心・半径が有限で、半径が非負でなければならない。違反時は `std::invalid_argument` を送出する。
- `point` が非有限、または（許容誤差の範囲で）球面上にない場合は `std::invalid_argument` を送出する。
- 半径 $0$ では中心自身を返す。
- 中心からの差分を正規化して計算し、中間値のオーバーフローを避ける。結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
