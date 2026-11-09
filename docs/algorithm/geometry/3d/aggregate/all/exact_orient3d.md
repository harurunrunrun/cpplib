---
title: Exact Integer Orient3D (整数三次元向きの正確判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/exact_orient3d.hpp
---

このヘッダは `IntegerPoint3` 型と整数点に対する述語の 2 つの個別ヘッダを提供する、互換のための集約ヘッダである。

## 公開型

- `IntegerPoint3`: `std::array<std::int64_t, 3>` の別名。

## API

```cpp
int exact_orient3d(
    const IntegerPoint3& first,
    const IntegerPoint3& second,
    const IntegerPoint3& third,
    const IntegerPoint3& fourth
);
```

行列式 $\det(b - a, c - a, d - a)$（$a, b, c, d$ はそれぞれ `first`, `second`, `third`, `fourth`）の正確な符号を $-1, 0, 1$ のいずれかで返す。

## API 別の時間計算量・空間計算量

座標の差の絶対値がすべて $2^{40}$ 以下なら、`__int128` による高速経路で時間・追加空間ともに $O(1)$。それ以外はビット長を $B$ として多倍長整数演算の時間がかかり、追加空間は $O(B)$。

## 注意点

- `std::int64_t` の座標の全範囲で正確である。全範囲の座標差 3 個の積は `__int128` に収まらないため、安全な範囲でだけ `__int128` を使い、それ以外はライブラリ内の `ExactInteger` に切り替える。外部の多倍長整数ライブラリには依存しない。
- 退化した入力（4 点が同一平面上）には 0 を返す。
