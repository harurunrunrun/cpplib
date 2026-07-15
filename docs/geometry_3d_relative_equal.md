---
title: 3D Relative Equality (三次元幾何の相対誤差比較)
documentation_of: ../src/algorithm/geometry/3d/relative_equal.hpp
---

## API

```cpp
bool relative_equal(first, second, relative_tolerance, absolute_tolerance)
```

`|first-second| <= max(absolute_tolerance, relative_tolerance * max(|first|,|second|))` なら `true` を返す。既定値は両方 `GEOMETRY3D_EPS`。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

比較値と許容誤差は有限、許容誤差は非負でなければならず、違反時は `std::invalid_argument`。座標差の符号など位相を決める用途には、対象式の次数に対応するscaleを渡す `scale_aware_sign` またはadaptive predicateを使う。
