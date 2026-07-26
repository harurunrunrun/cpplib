---
title: Expansion from Two Scalars (2スカラーからの浮動小数点展開)
documentation_of: ../../../../src/algorithm/geometry/3d/geometry3d_expansion_from_two_scalars.hpp
---

2個の `long double` の和・差・積を丸め誤差成分ごと保持する展開へ変換します。

## API

- `geometry3d_two_sum(first, second)`: 厳密な和を表す高々2成分の展開を返します。
- `geometry3d_two_diff(first, second)`: 厳密な差を表す高々2成分の展開を返します。
- `geometry3d_two_product(first, second)`: `std::fma` を用いて厳密な積を表す高々2成分の展開を返します。

## API別の時間計算量・空間計算量

すべて時間・返却値を除く追加空間ともに $O(1)$ です。

## 注意点

入力は有限値でなければなりません。非有限値は `std::invalid_argument`、演算結果のoverflowは `std::overflow_error` になります。