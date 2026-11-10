---
title: Safe Normalized Scalar Difference (安全な正規化スカラー差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/long_double_safe_normalized_difference.hpp
---

## API

- `geometry3d_safe_normalized_difference(left, right, positive_scale)`: オーバーフローを避けて $(\mathit{left} - \mathit{right}) / \mathit{scale}$ を返す。符号が異なる 2 数は、先にそれぞれを `positive_scale` で割ってから差をとる。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 引数は有限で、`positive_scale` は正でなければならない。違反時は `std::invalid_argument` を送出する。
- 非零の差がアンダーフローで $0$ になり表現できない場合は `std::overflow_error` を送出する。
