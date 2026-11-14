---
title: Four Russians Boolean Matrix Multiplication (Four Russians 法による Boolean 行列積)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/bitset/four_russians_boolean_matrix.hpp
---

内側の次元を幅 $t$ のブロックに分け、各ブロックのすべてのビットパターンに対応する行の OR を表に前計算して引くことで、Boolean 半環上の行列積を高速化する。名前空間は `dynamic_programming` である。

## `BooleanMatrix`

0/1 を `unsigned char` で保持する二次元の `std::vector`（`std::vector<std::vector<unsigned char>>`）。

## `four_russians_boolean_matrix_multiply(first, second, block_size = 0)`

$m\times k$ と $k\times n$ の Boolean 行列の積（$m \times n$）を返す。
`block_size == 0` では、$k$ のビット幅 $\lfloor\log_2 k\rfloor + 1$ を $2$ で割って切り捨てた値と $1$ の大きい方を $t$ に選ぶ。

## 時間計算量

内部では表と結果の行を 64 ビット語に詰めて語単位で OR する。語長を $w = 64$ として $O(mkn/(tw) + (k/t)2^t n/w + mk + kn + mn)$。

## 空間計算量

追加領域 $O(2^t n/w + kn/w + mn)$（表、詰めた行列と戻り値）。

## 注意点

- 要素は $0$ または $1$ で、各行列は長方形であり、内側の次元が一致しなければならない。
- 表が大きくなりすぎるのを防ぐため、`block_size <= 20` でなければならない。
- 不正な入力には `std::invalid_argument` を送出する。
- いずれかの次元が $0$ なら、零行列（$m \times n$）を返す。
