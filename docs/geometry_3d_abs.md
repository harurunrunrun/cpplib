---
title: 3D Vector Length (3次元ベクトル長)
documentation_of: ../src/algorithm/geometry/3d/scalar/abs.hpp
---

## API

- `abs(point)`: `point` のユークリッド長を返す。成分を直接二乗せず、スケーリングされた `hypot` で計算する。

## API別の時間計算量・空間計算量

- `abs(point)`: 時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- 全成分が有限でなければ `std::invalid_argument` を送出する。
- 入力は有限でも、真の長さが `long double` で表現できなければ `std::overflow_error` を送出する。
- `LDBL_MIN` 未満の非ゼロ値を含むベクトルでも、表現可能な長さをゼロへ丸めずに計算する。
