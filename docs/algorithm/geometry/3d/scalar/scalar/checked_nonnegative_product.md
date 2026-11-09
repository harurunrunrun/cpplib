---
title: Checked Nonnegative Product (検査付き非負積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/checked_nonnegative_product.hpp
---

## API

- `geometry3d_detail::checked_nonnegative_product(factors, overflow_message)`: 非負の因子の列 `factors`（`std::initializer_list<long double>`）の積を、仮数と 2 進指数に分けて蓄積しながら計算する。中間積が一時的に表現範囲を超えても、最終結果が表現可能なら正しく返す。

## API別の時間計算量・空間計算量

- 因子の個数を $k$ として、時間 $O(k)$、追加領域 $O(1)$。

## 注意点

- 因子は有限かつ非負でなければならず、違反時は `std::invalid_argument` を送出する。
- 因子に $0$ があればその時点で $0$ を返す（それ以降の因子は検査しない）。
- 結果が表現範囲を超える場合は、`overflow_message` をメッセージとする `std::overflow_error` を送出する。
