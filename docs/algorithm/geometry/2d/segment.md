---
title: 2D Segment (二次元線分)
documentation_of: ../../../../src/algorithm/geometry/2d/segment.hpp
---

2端点を持つ閉線分を表す。

## API

- `Segment{a, b}`: 端点 `a`, `b` から構築する。
- `supporting_line(segment)`: 線分の2端点を通る無限直線を返す。

## API別の時間計算量・空間計算量

- 構築、field参照、`supporting_line`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`a == b` の退化線分を許すかは利用するAPIごとに異なる。無限直線とは別型である。
`supporting_line` は退化線分にも値を返すため、返された直線を使うAPIの前提を別途満たす必要がある。
