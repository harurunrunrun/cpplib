---
title: Four Russians Boolean Matrix Multiplication (Four Russians法によるBoolean行列積)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/bitset/four_russians_boolean_matrix.hpp
---

小blockの全bit patternに対応する行ORを表引きし、Boolean semiring上の行列積を高速化する。

## `BooleanMatrix`

0/1を `unsigned char` で保持する二次元vector。

## `four_russians_boolean_matrix_multiply(first, second, block_size=0)`

$m\times k$ と $k\times n$ のBoolean行列積を返す。block幅を $t$ とすると時間計算量 $O(mkn/t + (k/t)2^t n)$、追加空間計算量 $O(2^t n+mn)$。`block_size == 0` では $t=\max(1,\lfloor\log_2 k\rfloor/2)$ を選ぶ。

## 注意点

要素は0または1、内側次元は一致しなければならない。表の暴発を防ぐため `block_size <= 20`。不正入力には `std::invalid_argument` を送出する。
