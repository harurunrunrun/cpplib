---
title: 3D Vector Length (三次元ベクトル長)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point/abs.hpp
---

## API

- `abs(point)`: `point` のユークリッド長を返す。成分を直接二乗して足すのではなく、3 引数の `std::hypot` で計算する。

## API別の時間計算量・空間計算量

- `abs(point)`: 時間・追加領域ともに $O(1)$。

## 注意点

- 全成分が有限でなければ `std::invalid_argument` を送出する。
- 入力が有限でも、長さが `long double` で表現できなければ `std::overflow_error` を送出する。
- `std::hypot` はスケーリングして計算するため、`LDBL_MIN` 未満の非零成分を含むベクトルでも、表現可能な長さを $0$ に丸めずに求められる。
