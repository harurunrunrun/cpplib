---
title: 3D Line Direction (三次元直線方向)
documentation_of: ../src/algorithm/geometry/geometry_3d/line3_direction.hpp
---

## API

- `line3_direction(line)`: `line.b - line.a` を有効性検査して返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化直線には `std::invalid_argument` を送出する。戻り値は単位化されない。
