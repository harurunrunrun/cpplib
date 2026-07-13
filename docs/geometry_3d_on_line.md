---
title: Point on 3D Line (三次元直線上判定)
documentation_of: ../src/algorithm/geometry/3d/on_line.hpp
---

## API

- `on_line(line, point)`: 点が無限直線上にあるか判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

退化直線には `std::invalid_argument` を送出する。
