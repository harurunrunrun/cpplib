---
title: 3D Zero Vector Check (三次元零ベクトル判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point/is_zero_vector.hpp
---

## API

```cpp
bool is_zero_vector(
    const Point3& vector,
    long double reference_scale = 1.0L,
    long double relative_tolerance = GEOMETRY3D_EPS,
    long double absolute_tolerance = GEOMETRY3D_EPS
)
```

最大絶対成分を、`reference_scale` と許容誤差を用いて `scale_aware_sign` で判定し、符号が $0$ ならゼロベクトルとみなして `true` を返す。

## API 別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- ベクトルは有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 正規化の前に使う場合は、処理対象の代表的な長さを `reference_scale` として渡す。
- `false` が返っても、正規化後の計算がオーバーフローしないことまでは保証しない。
