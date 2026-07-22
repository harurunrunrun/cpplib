---
title: Orient3D Determinant (三次元向き付き体積式)
documentation_of: ../src/algorithm/geometry/3d/predicate/orient3d.hpp
---

## API

```cpp
long double orient3d(first, second, third, fourth)
```

`det(second-first, third-first, fourth-first)`、すなわち符号付き四面体体積の6倍を返す。正なら右手系、負なら逆向き、0なら同一平面上を表す。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

入力は有限でなければならず、違反時は `std::invalid_argument`。差または結果を `long double` で表せなければ `std::overflow_error`。これは構成用の近似値APIであり、丸めに左右されない符号には `adaptive_orient3d`、整数座標には `exact_orient3d` を使う。退化入力は0を返す。
