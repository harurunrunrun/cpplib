---
title: Closest Point on 3D Circle (三次元円周上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/circle_point/circle_closest_point.hpp
---

## API

- `closest_point(circle, point)`: 円周上で `point` に最も近い点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `Circle3` は中心・法線・半径が有限、法線が非零、半径が非負でなければならない。違反時、および `point` が非有限のときは `std::invalid_argument` を送出する。
- 半径 $0$ では中心を返す。
- 正半径で `point` が円の中心軸（中心を通り法線に平行な直線）上にある場合は、最近点が一意でないため `std::domain_error` を送出する。
- 中心からの差分を正規化して計算し、中間値のオーバーフローを避ける。結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
