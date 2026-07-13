---
title: 3D Vector Angle (三次元ベクトル角)
documentation_of: ../src/algorithm/geometry/3d/angle.hpp
---

## API

- `angle(first, second)`: 2ベクトル間の角を $[0,\pi]$ ラジアンで返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

いずれかが零ベクトルなら `std::invalid_argument` を送出する。
