---
title: 3D Line Direction (三次元直線の方向)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/line/line3_direction.hpp
---

## API

- `line3_direction(line)`: 方向ベクトル `line.b - line.a` を退化検査したうえで返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 方向ベクトルの長さが `GEOMETRY3D_EPS`（$10^{-10}$） 以下なら退化直線とみなし、`std::invalid_argument` を送出する（絶対許容誤差による判定）。
- 戻り値は単位化されない。
