---
title: 3D Line Intersection (三次元直線同士の共通部分)
documentation_of: ../src/algorithm/geometry/3d/query/line_line_intersection.hpp
---

## API

- `line_line_intersection(first, second)`: 共通部分を `LinearIntersection3` で返す。空、点、一致直線のいずれかである。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

平行で異なる直線およびねじれの位置では空集合を返す。退化直線には `std::invalid_argument` を送出する。
