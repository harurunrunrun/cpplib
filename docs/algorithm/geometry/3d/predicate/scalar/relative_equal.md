---
title: 3D Relative Equality (三次元幾何の相対誤差比較)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/scalar/relative_equal.hpp
---

## API

```cpp
bool relative_equal(
    long double first,
    long double second,
    long double relative_tolerance = GEOMETRY3D_EPS,
    long double absolute_tolerance = GEOMETRY3D_EPS
);
```

`first` を $a$、`second` を $b$、`relative_tolerance` を $r$、`absolute_tolerance` を $t$ として、$\lvert a - b \rvert \le \max(t, r \cdot \max(\lvert a \rvert, \lvert b \rvert))$ なら `true` を返す。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 比較する値と許容誤差は有限で、許容誤差は非負でなければならない。違反時は `std::invalid_argument` を送出する。
- $a = b$ なら常に `true` を返す。$r \cdot \max(\lvert a \rvert, \lvert b \rvert)$ がオーバーフローして有限でなくなった場合も `true` を返す。
- 座標差の符号など位相を決める用途には、対象の式の次数に対応したスケールを渡す `scale_aware_sign` か、適応的精度の述語を使う。
