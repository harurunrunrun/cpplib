---
title: Estimate Floating-Point Expansion (浮動小数点展開の近似値)
documentation_of: ../../../../src/algorithm/geometry/3d/geometry3d_expansion_estimate.hpp
---

浮動小数点展開の全成分を加算した近似値を返します。

## API

- `geometry3d_expansion_estimate(expansion)`: 成分順に加算した `long double` を返します。

## API別の時間計算量・空間計算量

成分数を $N$ とすると時間 $O(N)$、追加空間 $O(1)$ です。

## 注意点

厳密値ではなく `long double` への丸めを含む近似値です。非有限成分は `std::invalid_argument`、加算結果のoverflowは `std::overflow_error` になります。