---
title: Estimate Floating-Point Expansion (浮動小数点展開の近似値)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/numeric/geometry3d_expansion_estimate.hpp
---

浮動小数点展開（複数の浮動小数点数の和で実数を厳密に表す表現）の全成分を加算した近似値を返す。

## API

- `geometry3d_expansion_estimate(expansion)`: 成分を順に加算した `long double` を返す。

## API別の時間計算量・空間計算量

- 成分数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

- 厳密値ではなく、`long double` への丸めを含む近似値である。
- 非有限な成分があれば `std::invalid_argument` を、加算結果がオーバーフローすれば `std::overflow_error` を送出する。
