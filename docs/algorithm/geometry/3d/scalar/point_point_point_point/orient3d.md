---
title: Orient3D Determinant (三次元向き付き体積式)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point_point_point/orient3d.hpp
---

## API

```cpp
long double orient3d(first, second, third, fourth)
```

`det(second - first, third - first, fourth - first)`、すなわち符号付き四面体体積の 6 倍を返す。正なら 4 点が右手系の向き、負なら逆向き、$0$ なら同一平面上にあることを表す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 入力は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 座標の差や結果を `long double` で表現できなければ `std::overflow_error` を送出する。
- 差を最大成分で正規化してから行列式を計算し、スケールを指数で復元する。4 点が一致する場合は $0$ を返す。
- これは構成計算用の近似値 API であり、同一平面に近い入力では丸めにより符号が変わり得る。丸めに左右されない符号が必要なら `adaptive_orient3d`、整数座標なら `exact_orient3d` を使う。
