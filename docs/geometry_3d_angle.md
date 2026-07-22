---
title: 3D Vector Angle (三次元ベクトル角)
documentation_of: ../src/algorithm/geometry/3d/scalar/angle.hpp
---

## API

- `angle(first, second)`: 2ベクトル間の角を $[0,\pi]$ ラジアンで返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零ベクトルまたは非有限ベクトルには `std::invalid_argument` を送出する。両ベクトルを
overflow・underflowしない方法で単位化し、`atan2(abs(cross), dot)` から角度を求める。
このため一様な巨大・微小scaleでも積のoverflowや `acos` の範囲外丸めを避ける。
