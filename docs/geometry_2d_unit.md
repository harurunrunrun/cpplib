---
title: 2D Unit Vector (二次元単位ベクトル)
documentation_of: ../src/algorithm/geometry/2d/point/unit.hpp
---

## API

- `unit(point)`: `point` と同方向の単位ベクトルを返す。

## API別の時間計算量・空間計算量

- `unit`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

許容誤差の範囲で零ベクトルなら `std::invalid_argument` を送出する。
