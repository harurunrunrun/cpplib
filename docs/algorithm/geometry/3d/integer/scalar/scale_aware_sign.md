---
title: 3D Scale-Aware Sign (三次元幾何のスケール付き符号判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/scalar/scale_aware_sign.hpp
---

## API

```cpp
int scale_aware_sign(value, scale, relative_tolerance = GEOMETRY3D_EPS, absolute_tolerance = GEOMETRY3D_EPS)
```

`max(absolute_tolerance, relative_tolerance * scale)` 以下の絶対値を零とみなし、`value` の符号を `-1`, `0`, `1` で返す。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

全引数は有限で、`scale` と許容誤差は非負でなければならない。違反時は `std::invalid_argument` を、許容幅の積 `relative_tolerance * scale` がオーバーフローする場合は `std::overflow_error` を送出する。
`scale` は `value` と同じ次元で渡す。たとえば長さなら代表長、面積の式なら代表長の 2 乗、体積の式なら 3 乗を渡す。
