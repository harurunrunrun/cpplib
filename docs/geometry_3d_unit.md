---
title: 3D Unit Vector (三次元単位ベクトル)
documentation_of: ../src/algorithm/geometry/geometry_3d/unit.hpp
---

## API

- `unit(point)`: 同方向の単位ベクトルを返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

許容誤差の範囲で零ベクトルなら `std::invalid_argument` を送出する。
