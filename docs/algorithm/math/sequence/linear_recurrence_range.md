---
title: Consecutive Linear Recurrence Terms (線形漸化式の連続区間) [consecutive_terms_of_linear_recurrent_sequence]
documentation_of: ../../../../src/algorithm/math/sequence/linear_recurrence_range.hpp
---

`math::linear_recurrence_range<MOD>(initial, coefficient, start, count)` は、`initial[i]` $= a_i$ を初期項とする線形漸化式

$$
a_i=\sum_{j=0}^{d-1}\mathtt{coefficient}_j\,a_{i-1-j}
$$

の連続区間 $a_{\mathrm{start}}, \ldots, a_{\mathrm{start}+\mathrm{count}-1}$ を長さ `count` の列で返す。`start` は `std::uint64_t` である。
2 つの入力列の長さは一致させる必要があり、異なる場合は `std::invalid_argument` を送出する。空の初期列は零列を表す。
末尾の漸化式係数が $0$ のときも、母関数の多項式部分を分離して処理する。

## 時間計算量

母関数を $P/Q + R$（$R$ は多項式）と表し、$x^{-\mathrm{start}} P \bmod Q$ から区間の先頭に対応する分子を計算する。法多項式 $Q$ の反転多項式の逆元を 1 回だけ前計算し、冪の各段階で再利用する。
$M(n)$ を長さ $n$ の多項式乗算の時間として、時間 $O(M(d)\log(\mathrm{start}+1)+M(d+\mathrm{count}))$ である。

## 空間計算量

追加領域 $O(d+\mathrm{count})$（再帰の中間多項式を含めると $O(d\log(\mathrm{start}+1)+\mathrm{count})$）。

## 使用例

遠い位置から始まる長さ $M$ の区間和を求めるには、返された列の和をとればよい。累積和を作れば、取得した区間内の部分区間和にも答えられる。

## 注意点

多項式乗算に NTT を使うため、必要な 2 の冪の変換長が `MOD - 1` を割り切る必要がある。
