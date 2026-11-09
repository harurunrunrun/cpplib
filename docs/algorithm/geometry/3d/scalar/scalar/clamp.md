---
title: 3D Finite Clamp (三次元幾何の有限値クランプ)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/clamp.hpp
---

## API

```cpp
long double clamp(value, lower_bound, upper_bound)
```

有限な値 `value` を閉区間 `[lower_bound, upper_bound]` にクランプする。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 値または境界が非有限の場合や、下限が上限より大きい場合は `std::invalid_argument` を送出する。`NaN` を黙って境界値に変換することはない。
- 丸め誤差で `acos` の引数などがわずかに範囲外へ出る場合の補正に用いる。
