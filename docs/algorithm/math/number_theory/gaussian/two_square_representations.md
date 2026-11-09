---
title: Two Square Representations (二平方和の全列挙) [two_square_sum]
documentation_of: ../../../../../src/algorithm/math/number_theory/gaussian/two_square_representations.hpp
---

## API

- `math::two_square_representations(n)`: $a^2 + b^2 = n$ を満たす非負整数の順序付き組 $(a, b)$ をすべて、辞書順・重複なしの `std::vector<std::pair<uint64_t, uint64_t>>` で返す。`n` は `uint64_t` で、`n = 0` のときは $(0, 0)$ のみを返す。

原点からの距離の 2 乗が $n$ である格子点を第一象限（軸上を含む）で列挙することに相当する。$a \le b$ の組だけを取り出せば、順序を区別しない表示が得られる。

## アルゴリズム

$n$ を Pollard–Rho 法で素因数分解し、$p \equiv 1 \pmod 4$ は Cornacchia 法でガウス素数の積に分解する。$p \equiv 3 \pmod 4$ の指数が奇数なら解はなく、空の列を返す。ガウス整数の積を組み合わせて全表示を作り、座標の中間値には符号付き 128 bit 整数を用いる。

## 時間計算量・空間計算量

素因数分解の後は、出力の個数を $R$（$R \le 2\tau(n)$、$\tau$ は約数の個数）として時間 $O(R \log R)$、空間 $O(R)$。

## 注意点

GCC 13 / C++20 を想定する（`__int128_t` を使用）。
