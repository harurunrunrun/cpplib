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

非負整数上でmexにより定義されるNim積 `left` $\otimes$ `right` を返す。
通常のcarry-less多項式積を特定の既約多項式で割る表現とは一致しない。

初回呼び出しで、Fermat 2冪ごとの再帰公式から64×64個の基底積を求め、
各8-bit block対の積を表に展開する。その後は入力を8個のbyteへ分解し、
最大64個の表引き結果をxorする。

## 時間計算量

- 初回の表構築: 64-bit固定では約420万entryの構築
- 1回の積: 最大64回のtable lookupとxor、すなわち $O(1)$
- 100万回の積: $O(10^6)$

表は約32 MiB、基底表と作業領域は $O(1)$。

## 注意点

演算は `std::uint64_t` の全bit patternに対して定義される。初回呼び出しだけ
表構築コストが発生する。関数内staticの初期化はC++11以降thread-safeである。
