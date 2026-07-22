---
title: In-Sphere Determinant (三次元外接球内外式)
documentation_of: ../src/algorithm/geometry/3d/predicate/insphere.hpp
---

## API

```cpp
long double insphere(first, second, third, fourth, query)
```

非退化四面体の外接球に対し、頂点順に依存しない符号へ正規化したdeterminantを返す。正なら球内、0なら球面上、負なら球外。

## API別の時間計算量・空間計算量

時間・追加領域 $O(1)$。

## 注意点

入力は有限でなければならず、違反時は `std::invalid_argument`。四面体が許容誤差内で退化している場合も `std::invalid_argument`。結果のoverflowは `std::overflow_error`。これは構成用近似値であり、Delaunayの位相判定には `adaptive_insphere` または整数版 `exact_insphere` を使う。
