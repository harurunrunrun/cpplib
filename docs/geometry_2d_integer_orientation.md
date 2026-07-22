---
title: 2D Integer Orientation (二次元整数点向き判定)
documentation_of: ../src/algorithm/geometry/2d/predicate/integer_orientation.hpp
---

## API

`int integer_orientation(first, second, third)`

## 引数

向きを調べる同じ整数座標型の3点を渡す。

## 戻り値

反時計回りなら `1`、時計回りなら `-1`、共線なら `0`。

## API別の時間計算量・空間計算量

- `integer_orientation`: 固定256 bit演算で時間 $O(1)$、追加領域 $O(1)$。

## 注意点

座標差を `__int128` で求め、積と差を256 bitで求めるため `long long` の端点差にも対応する。
