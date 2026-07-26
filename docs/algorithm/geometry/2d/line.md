---
title: 2D Line (二次元直線)
documentation_of: ../../../../src/algorithm/geometry/2d/line.hpp
---

2点を通る無限直線を表す。

## API

- `Line{a, b}`: 通過点 `a`, `b` から構築する。

## API別の時間計算量・空間計算量

- 構築とfield参照は時間 $O(1)$、追加領域 $O(1)$。

## 注意点

直線を利用するAPIでは通常 `a != b` が必要。閉線分には `Segment` を使う。
