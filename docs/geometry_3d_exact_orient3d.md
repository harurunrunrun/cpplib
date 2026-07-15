---
title: Exact Integer Orient3D (整数三次元向きの正確判定)
documentation_of: ../src/algorithm/geometry/3d/exact_orient3d.hpp
---

## 公開型

- `IntegerPoint3`: `array<int64_t, 3>` の別名。

## API

```cpp
int exact_orient3d(first, second, third, fourth)
```

`det(second-first, third-first, fourth-first)` の正確な符号を `-1, 0, 1` で返す。

## API別の時間計算量・空間計算量

差の絶対値がすべて $2^{40}$ 以下なら時間・追加領域 $O(1)$ の `__int128` fast path。それ以外はbit長を $B$ として多倍長整数演算時間、追加領域 $O(B)$。

## 注意点

全 `int64_t` 座標域で正確。full-rangeの座標差3個の積は `__int128` に収まらないため、安全な範囲だけ `__int128` を使い、それ以外は `boost::multiprecision::cpp_int` へ切り替える。退化入力は0を返す。
