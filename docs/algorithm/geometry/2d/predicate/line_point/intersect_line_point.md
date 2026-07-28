---
title: Line-Point Incidence Test (直線と点の所属判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line_point/intersect_line_point.hpp
---

点が無限直線上にあるか判定する。

## API

- `intersect(line, point)`: `point` が `line` 上にあれば `true` を返す。

## API別の時間計算量・空間計算量

- `intersect`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`line.a == line.b` の退化直線には `std::invalid_argument` を送出する。判定には方向のscaleを考慮した許容誤差を用いる。
