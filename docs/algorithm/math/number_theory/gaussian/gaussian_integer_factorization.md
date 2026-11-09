---
title: Factorization of Gaussian Integers (ガウス整数の素因数分解)
documentation_of: ../../../../../src/algorithm/math/number_theory/gaussian/gaussian_integer_factorization.hpp
---

非零のガウス整数を、単数と正規化されたガウス素数の冪の積に分解する。`GaussianInteger` の定義とガウス整数の最大公約数 `gaussian_gcd` は `gaussian_integer.hpp` を参照。

## `GaussianPrimePower`

```cpp
math::GaussianPrimePower{prime, exponent}
```

素因子 `prime`（`GaussianInteger`）と正の指数 `exponent`（`int`）を保持する。`prime` は単数倍の不定性を除くため、`prime.real > 0 && prime.imag >= 0` を満たす同伴元へ正規化されている。等値比較を使用できる。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 構築、メンバ参照、等値比較 | $O(1)$ | $O(1)$ |

## `GaussianIntegerFactorization`

```cpp
math::GaussianIntegerFactorization{unit, factors}
```

次の分解を保持する。

$$
z=\mathtt{unit}\prod_j\mathtt{factors}[j].\mathtt{prime}^{\mathtt{factors}[j].\mathtt{exponent}}
$$

`unit` は $1, -1, i, -i$ のいずれかである。`factors` は `(prime.real, prime.imag)` の辞書順に並び、同じ素因子は一度だけ現れる。等値比較を使用できる。

$k$ を相異なるガウス素因子の個数とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 構築 | $O(k)$ | $O(k)$ |
| `unit` の参照 | $O(1)$ | $O(1)$ |
| `factors` の参照、等値比較 | $O(k)$ | $O(1)$ |

## `factorize_gaussian_integer`

```cpp
math::GaussianIntegerFactorization math::factorize_gaussian_integer(
    math::GaussianInteger value
);
```

`value` のガウス素因数分解を返す。ノルム $N = \mathrm{real}^2 + \mathrm{imag}^2$ を Pollard–Rho 法で素因数分解し、各素数 $p$ を次のように処理する。

- $p = 2$ は分岐し、$1+i$ を得る。
- $p \equiv 3 \pmod 4$ はガウス整数としても素数である。
- $p \equiv 1 \pmod 4$ は $\sqrt{-1} \bmod p$ から Cornacchia 法で $p = x^2 + y^2$ を復元し、$x+yi$ とその共役の同伴元に分ける。

得られた素因子で `value` を割り切れるだけ割って各指数と残りの単数を求める。

$N$ の Pollard–Rho 素因数分解にかかる時間を $F(N)$ とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| $p \equiv 1 \pmod 4$ ごとの分解 | 期待 $O(\log p)$ | $O(1)$ |
| 指数の抽出 | $O(\log N)$ | $O(1)$ |
| 因子の整列 | $O(k \log k)$ | $O(\log k)$ |
| 全体 | 期待 $O(F(N) + \log N + k \log k)$ | $O(k + \log N)$ |

通常は $F(N)$ が全体を支配する。Miller–Rabin の基底と Pollard–Rho の乱数シードは固定なので、同じ入力に対する戻り値は決定的である。

## 注意点

- `value == 0` の素因数分解は定義せず、`std::invalid_argument` を送出する。
- $N$ が `uint64_t` に収まらない場合は `std::overflow_error` を送出する。
- `value` が単数なら `factors` は空で、`unit == value` となる。
- 積や割り算の中間値には 128 bit 整数を用いる。戻り値の成分はすべて `long long` である。
