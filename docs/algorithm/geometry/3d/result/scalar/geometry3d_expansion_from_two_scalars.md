---
title: Expansion from Two Scalars (2スカラーからの浮動小数点展開)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/scalar/geometry3d_expansion_from_two_scalars.hpp
---

2 個の `long double` の和・差・積を、丸め誤差の成分まで保持した浮動小数点展開（`Geometry3DExpansion`）に変換する。

## API

- `geometry3d_two_sum(first, second)`: 厳密な和を表す高々 2 成分の展開を返す。
- `geometry3d_two_diff(first, second)`: 厳密な差を表す高々 2 成分の展開を返す。
- `geometry3d_two_product(first, second)`: `std::fma` を用いて、厳密な積を表す高々 2 成分の展開を返す。

成分は絶対値の小さい順（誤差項、主項の順）に並び、$0$ の成分は除く。結果が $0$ のときは `{0}` を返す。

## API別の時間計算量・空間計算量

いずれも時間・戻り値を除く追加空間ともに $O(1)$。

## 注意点

入力は有限値でなければならない。非有限値には `std::invalid_argument` を、演算結果がオーバーフローした場合は `std::overflow_error` を送出する。
