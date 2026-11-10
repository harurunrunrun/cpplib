---
title: 2D Unit Vector (二次元単位ベクトル)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/point/unit.hpp
---

## API

- `Point unit(const Point& point)`: `point` と同じ向きの単位ベクトルを返す。

## API別の時間計算量・空間計算量

- `unit`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

長さが `GEOMETRY_EPS` 以下（許容誤差の範囲で零ベクトル）の場合は `std::invalid_argument` を送出する。
