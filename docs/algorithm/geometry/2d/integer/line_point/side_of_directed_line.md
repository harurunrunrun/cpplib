---
title: Side of Directed Line (有向直線の左右判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/line_point/side_of_directed_line.hpp
---

## API

`int side_of_directed_line(const Line& line, const Point& point)`

## 引数

退化していない有向直線 `line`（`line.a` から `line.b` へ向かう）と、判定する点 `point` を渡す。

## 戻り値

左側なら `1`、右側なら `-1`、直線上なら `0`。

## API別の時間計算量・空間計算量

- `side_of_directed_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

方向ベクトルの長さが `GEOMETRY_EPS` 以下の退化直線では `std::invalid_argument` を送出する。
判定は、直線までの符号付き垂直距離を `GEOMETRY_EPS` と丸め誤差の和と比較して行う。
したがって退化判定に該当しない範囲では、方向ベクトル `line.b - line.a` を正の定数倍しても戻り値は変わらない。
