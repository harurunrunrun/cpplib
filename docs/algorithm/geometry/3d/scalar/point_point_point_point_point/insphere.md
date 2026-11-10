---
title: In-Sphere Determinant (三次元外接球内外式)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point_point_point_point/insphere.hpp
---

## API

```cpp
long double insphere(first, second, third, fourth, query)
```

非退化な四面体の外接球に対する `query` の位置を表す行列式を、頂点の順序（四面体の向き）に依存しない符号へ正規化して返す。正なら球の内部、$0$ なら球面上、負なら球の外部を表す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 入力は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 四面体が許容誤差内で退化している場合も `std::invalid_argument` を送出する。
- 座標の差や結果を `long double` で表現できなければ `std::overflow_error` を送出する。
- これは構成計算用の近似値であり、Delaunay 分割などの位相判定には `adaptive_insphere`、整数座標なら `exact_insphere` を使う。
