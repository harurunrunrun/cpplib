---
title: Consecutive Linear Recurrence Terms (線形漸化式の連続区間) [consecutive_terms_of_linear_recurrent_sequence]
documentation_of: ../../../../src/algorithm/math/sequence/linear_recurrence_range.hpp
---

`math::linear_recurrence_range<MOD>(initial,coefficient,start,count)` は

$$a_i=\sum_{j=0}^{d-1}coefficient[j]\,a_{i-1-j}$$

の連続区間 `a[start],...,a[start+count-1]` を返す。2入力列の長さは一致させる。`start` は `uint64_t`。末尾の漸化式係数が0のときも、母関数の多項式部分を分離して処理する。

## 時間計算量

母関数を `P/Q+R` と表し、`x^(-start)*P mod Q` から区間先頭の分子を計算する。法多項式の反転逆数を1回だけ前計算し、冪の各段階で再利用する。時間 $O(M(d)\log(start+1)+M(d+count))$、追加領域 $O(d+count)$ (再帰中間多項式を含む場合 $O(d\log(start+1)+count)$)。

類題: 遠い位置から長さMの区間和を答える。返された列の累積和を使えば同じ取得区間内の部分区間和も処理できる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
