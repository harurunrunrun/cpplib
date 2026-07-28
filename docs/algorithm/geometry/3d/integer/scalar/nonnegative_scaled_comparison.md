---
title: Nonnegative Scaled Comparison (倍率付き非負値比較)
documentation_of: ../../../../../../src/algorithm/geometry/3d/integer/scalar/nonnegative_scaled_comparison.hpp
---

## API

- `geometry3d_compare_nonnegative_scaled(left_value, left_scale, right_value, right_scale)` は二つの積を直接作らず比較して -1、0、1 を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

値は有限かつ非負、倍率は有限かつ正でなければならない。違反時は `std::invalid_argument` を送出する。
