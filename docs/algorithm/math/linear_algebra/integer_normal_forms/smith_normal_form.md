---
title: Smith Normal Form (Smith標準形)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/integer_normal_forms/smith_normal_form.hpp
---

整数行列 $A$ を $UAV=D$ と分解する。$U, V$ はユニモジュラ行列、$D$ は非負の対角成分が順に割り切れる（$d_i \mid d_{i+1}$）対角行列である。API は名前空間 `linear_algebra` にある。

## `SmithNormalFormResult<Integer>`

```cpp
template<class Integer>
struct SmithNormalFormResult {
    std::vector<std::vector<Integer>> diagonal;   // D
    std::vector<std::vector<Integer>> left;       // U
    std::vector<std::vector<Integer>> right;      // V
    std::size_t rank = 0;
};
```

`diagonal` が $D$、`left` が $U$、`right` が $V$、`rank` が 0 でない対角成分の個数である。

## `smith_normal_form(matrix)`

Euclid の互除法型の行・列基本変形で $m \times n$ 行列の Smith 標準形を求める。

## 時間計算量

$r=\min(m,n)$、入力の成分の絶対値の上限を $M$ としたとき、通常 $O(rmn\log M)$ 回程度の整数演算を行う。整数の bit 長の増大を含む bit 計算量は入力に依存する。空間計算量は $O(m^2+n^2+mn)$。

## 注意点

- `Integer` は符号付き整数と同様の比較・四則演算・剰余を提供する必要がある。固定長整数では基本変形の中間値が範囲内に収まることが前提である。
- 行の長さがそろっていない入力には `std::invalid_argument` を送出する。
