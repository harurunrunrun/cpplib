---
title: 3D Finite Clamp (三次元幾何の有限値クランプ)
documentation_of: ../src/algorithm/geometry/3d/scalar/clamp.hpp
---

## API

```cpp
long double clamp(value, lower_bound, upper_bound)
```

有限な値を閉区間 `[lower_bound, upper_bound]` へクランプする。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

値または境界が非有限、あるいは下限が上限より大きい場合は `std::invalid_argument`。`NaN` を境界値へ黙って変換しない。丸め誤差で `acos` の引数などが僅かに範囲外へ出た場合の構成計算に用いる。
