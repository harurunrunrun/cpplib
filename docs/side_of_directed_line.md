---
title: Side of Directed Line (有向直線の左右判定)
documentation_of: ../src/algorithm/geometry/2d/side_of_directed_line.hpp
---

## API

`int side_of_directed_line(line, point)`

## 引数

有向非退化直線 `a -> b` と判定点を渡す。

## 戻り値

左側なら `1`、右側なら `-1`、直線上なら `0`。

## API別の時間計算量・空間計算量

- `side_of_directed_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

退化直線では `invalid_argument`。判定誤差は直線までの符号付き垂直距離に対する `GEOMETRY_EPS` と丸め誤差に従う。
退化判定に該当しない範囲では、方向ベクトル `line.b - line.a` を正の定数倍しても戻り値は変わらない。
