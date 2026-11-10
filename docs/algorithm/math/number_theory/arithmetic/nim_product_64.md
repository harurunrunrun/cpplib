---
title: Nim Product 64 (64-bit Nim積) [nim_product_64]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/nim_product_64.hpp
---

## `math::nim_product_64`

```cpp
std::uint64_t math::nim_product_64(
    std::uint64_t left,
    std::uint64_t right
)
```

非負整数上で mex により定義される Nim 積 `left` $\otimes$ `right` を返す。通常の繰り上がりなし（carry-less）多項式積を特定の既約多項式で割る表現とは一致しない。

初回の呼び出しで、Fermat 2 冪（$2^{2^k}$）ごとの再帰公式から $64\times 64$ 個の基底同士の積を求め、各 8 bit ブロックの組の積を表に展開する。その後は入力を 8 個のバイトに分解し、最大 64 個の表引きの結果を xor する。

## 時間計算量

- 初回の表構築: 64 bit 固定で約 420 万要素の構築
- 1 回の積: 最大 64 回の表引きと xor、すなわち $O(1)$
- 100 万回の積: $O(10^6)$

表は約 32 MiB、基底表と作業領域は $O(1)$。

## 注意点

- 演算は `std::uint64_t` のすべてのビットパターンに対して定義される。
- 初回の呼び出しでだけ表の構築コストが発生する。関数内 `static` 変数の初期化は C++11 以降スレッドセーフである。
