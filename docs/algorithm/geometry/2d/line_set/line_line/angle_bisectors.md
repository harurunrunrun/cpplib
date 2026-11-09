---
title: Angle Bisectors of Two Lines (二直線の角の二等分線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/line_set/line_line/angle_bisectors.hpp
---

## API

`std::array<Line, 2> angle_bisectors(const Line& first, const Line& second)`

## 引数

交差する 2 本の非退化な直線を渡す。

## 戻り値

交点を通り、互いに直交する 2 本の角の二等分線を返す。
各入力直線の単位方向ベクトルを、$x$ 成分が正（0 なら $y$ 成分が正）になるよう向きをそろえ、その和の方向の直線、差の方向の直線の順に返す。

## API別の時間計算量・空間計算量

- `angle_bisectors`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線では `std::invalid_argument` を、平行または一致する直線では `std::domain_error` を送出する。
平行の判定は、正規化した方向ベクトル同士の外積（角度の正弦）を `GEOMETRY_EPS` と比較して行う。
