---
title: Factorization of Gaussian Integers (ガウス整数の素因数分解) [factorization_of_gaussian_integers]
documentation_of: ../src/algorithm/math/gaussian_integer_factorization.hpp
---

非零のGauss整数を、単数と正規化されたGauss素数の冪へ分解する。
`GaussianInteger`の定義とGauss整数上の最大公約数は
[GCD of Gaussian Integers](gcd_of_gaussian_integers.md)を参照する。

## `GaussianPrimePower`

```cpp
math::GaussianPrimePower{prime, exponent}
```

素因子`prime`と正の指数`exponent`を保持する。`prime`は単数倍の不定性を
除くため`prime.real > 0 && prime.imag >= 0`を満たすassociateへ正規化される。
等値比較を使用できる。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 構築、メンバ参照、等値比較 | $O(1)$ | $O(1)$ |

## `GaussianIntegerFactorization`

```cpp
math::GaussianIntegerFactorization{unit, factors}
```

次の分解を保持する。

$$
z=\mathtt{unit}\prod_j\mathtt{factors}[j].\mathtt{prime}^{
\mathtt{factors}[j].\mathtt{exponent}}
$$

`unit`は`1`, `-1`, `i`, `-i`のいずれかである。`factors`は
`(prime.real, prime.imag)`の辞書順で、同じ素因子は一度だけ現れる。
等値比較を使用できる。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 構築 | $O(k)$ | $O(k)$ |
| `unit`参照 | $O(1)$ | $O(1)$ |
| `factors`参照、等値比較 | $O(k)$ | $O(1)$ |

ここで$k$は異なるGauss素因子の個数である。

## `factorize_gaussian_integer`

```cpp
math::GaussianIntegerFactorization math::factorize_gaussian_integer(
    math::GaussianInteger value
);
```

`value`のGauss素因数分解を返す。$N=value.real^2+value.imag^2$を
Pollard--Rho法で整数素因数分解し、整数素数$p$を次のように処理する。

- $p=2$はramifyし、$1+i$を得る。
- $p\equiv3\pmod4$はGauss整数上でも素数である。
- $p\equiv1\pmod4$はCornacchia法で$p=x^2+y^2$を復元し、
  $x+yi$とその共役のassociateへ分ける。

得た素因子で`value`を正確に除算してそれぞれの指数と残る単数を求める。

整数$N$のPollard--Rho素因数分解時間を$F(N)$、異なるGauss素因子数を
$k$とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| $p\equiv1\pmod4$ごとの分解 | 期待$O(\log p)$ | $O(1)$ |
| 指数の抽出 | $O(\log N)$ | $O(1)$ |
| 因子の整列 | $O(k\log k)$ | $O(\log k)$ |
| 全体 | 期待$O(F(N)+\log N+k\log k)$ | $O(k+\log N)$ |

`F(N)`は通常この処理全体を支配する。Miller--Rabinの基底とPollard--Rhoの
乱数seedは固定されているため、同じ入力に対する戻り値は決定的である。

## 注意点

- `value == 0`の素因数分解は定義せず、`invalid_argument`を送出する。
- $N$が`uint64_t`に収まらない場合は`overflow_error`を送出する。
- `value`が単数なら`factors`は空で、`unit == value`となる。
- 成分、積、正確な除算の中間値には128bit整数を用いる。戻り値の全成分は
  `long long`である。
