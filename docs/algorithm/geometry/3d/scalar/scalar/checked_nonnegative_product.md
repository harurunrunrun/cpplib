---
title: Checked Nonnegative Product (検査付き非負積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/checked_nonnegative_product.hpp
---

## API

- `geometry3d_detail::checked_nonnegative_product(factors, message)` は指数を分離して非負因子の積を計算する。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

因子は有限かつ非負でなければならない。違反時は `std::invalid_argument`、結果が表現範囲を超える場合は指定メッセージの `std::overflow_error` を送出する。
