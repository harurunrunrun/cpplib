---
title: 3D Scale-Aware Sign (三次元幾何のスケール付き符号判定)
documentation_of: ../src/algorithm/geometry/3d/scale_aware_sign.hpp
---

## API

```cpp
int scale_aware_sign(value, scale, relative_tolerance, absolute_tolerance)
```

`max(absolute_tolerance, relative_tolerance * scale)` を零近傍とし、`value` の符号を `-1, 0, 1` で返す。既定の許容誤差は `GEOMETRY3D_EPS`。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

全引数は有限、`scale` と許容誤差は非負でなければならない。違反時は `std::invalid_argument`、許容幅の積がoverflowする場合は `std::overflow_error`。`scale` は、長さなら代表長、面積式なら代表長の二乗、体積式なら三乗のように、`value` と同じ次元で渡す。
