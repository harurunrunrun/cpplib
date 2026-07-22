---
title: 3D Zero Vector Check (三次元零ベクトル判定)
documentation_of: ../src/algorithm/geometry/3d/predicate/is_zero_vector.hpp
---

## API

```cpp
bool is_zero_vector(vector, reference_scale, relative_tolerance, absolute_tolerance)
```

最大絶対成分を `scale_aware_sign` で判定する。`reference_scale` の既定値は1、許容誤差の既定値は `GEOMETRY3D_EPS`。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

ベクトルは有限でなければならず、違反時は `std::invalid_argument`。正規化前には、処理対象の代表長を `reference_scale` として渡す。`false` でも正規化後の計算がoverflowしないことまでは保証しない。
