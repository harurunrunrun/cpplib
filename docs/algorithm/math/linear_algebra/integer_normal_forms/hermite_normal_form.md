---
title: Row Hermite Normal Form (行Hermite標準形)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/integer_normal_forms/hermite_normal_form.hpp
---

整数の行基本変形だけを用いて、$UA=H$ となる行 Hermite 標準形 $H$ を構成する。API は名前空間 `linear_algebra` にある。

## `HermiteNormalFormResult<Integer>`

```cpp
template<class Integer>
struct HermiteNormalFormResult {
    std::vector<std::vector<Integer>> matrix;       // H
    std::vector<std::vector<Integer>> left;         // U
    std::vector<std::size_t> pivot_columns;
};
```

`matrix` が $H$、`left` がユニモジュラ行列 $U$、`pivot_columns` が $H$ の各非零行のピボット列である。

## `row_hermite_normal_form(matrix)`

$m \times n$ 行列について、ピボット列の下側の成分を Euclid の互除法で 0 にし、上側の成分を $[0, \text{pivot})$ の範囲に正規化する。

## 時間計算量

階数を $r$、入力の成分の絶対値の上限を $M$ とすると、通常 $O(rmn\log M)$ 回程度の整数演算を行う。空間計算量は $O(m^2+mn)$。

## 注意点

- `Integer` は符号付き整数と同様の比較・四則演算・剰余を提供する必要がある。固定長整数では中間値のオーバーフローに注意する。
- 行の長さがそろっていない入力には `std::invalid_argument` を送出する。
