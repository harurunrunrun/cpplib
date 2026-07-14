---
title: Basic Number Theory (初等整数論) [ALDS1_1_B] [enumerate_quotients] [NTL_1_B]
documentation_of: ../src/algorithm/math/basic_number_theory.hpp
---

整数まわりの基本関数。

## 関数

- `math::to_base_digits(n, base)`
    - `n` を `base` 進数の桁列に変換する。上位桁から並ぶ。
- `math::from_base_digits(digits, base)`
    - `base` 進数の桁列を整数に戻す。
- `math::divisors(n)`
    - `n` の正の約数を昇順で返す。
- `math::quotient_ranges(n)`
    - `floor(n / i)` が等しい半開区間 `[l, r)` を列挙する。
- `math::ext_gcd(a, b)`
    - `a x + b y = gcd(a, b)` となる `{g, x, y}` を返す。
- `math::gcd(a, b)`
    - 最大公約数を返す。
- `math::lcm(a, b)`
    - 最小公倍数を返す。
- `math::isqrt(n)`
    - `floor(sqrt(n))` を返す。
- `math::pow_mod(x, n, mod)`
    - `x^n mod mod` を返す。
- `math::inv_mod(a, mod)`
    - `mod` が素数でなくてもよい場合の逆元を返す。
- `math::inv_mod_prime(a, mod)`
    - `mod` が素数の場合の逆元を返す。
## 戻り値型

### `ExtGcdResult`

- `g`: 非負の $\gcd(a,b)$。
- `x`, `y`: 元の符号を含めて $ax+by=g$ を満たす係数。
- `ext_gcd(0,0)` では3fieldとも0。

### `QuotientRange`

- `l`, `r`: 半開区間 $[l,r)$。
- `value`: 区間内のすべての $i$ に対する $\lfloor n/i\rfloor$。

## API詳細

### `abs_ll(x)`

`x` の絶対値を返す。

### `gcd(a, b)` / `lcm(a, b)`

符号を無視した最大公約数・最小公倍数を返す。`gcd(0,0)` と、
どちらかが0の `lcm` は0。`lcm` の乗算overflowは検査しない。

### `ext_gcd(a, b)`

`ExtGcdResult` を返す。`g` は非負で、$ax+by=g$ を満たす。

### `pow_mod(x, n, mod)`

$x^n\bmod mod$ を $[0,mod)$ で返す。`n >= 0`、`mod > 0` が必要。
条件を満たさない場合は `runtime_error` を送出する。

### `inv_mod(a, mod)`

$a$ の `mod` における乗法逆元を $[0,mod)$ で返す。`mod > 0` かつ
$\gcd(a,mod)=1$ が必要。逆元が存在しない場合は `runtime_error` を送出する。

### `inv_mod_prime(a, mod)`

Fermatの小定理で逆元を返す。`mod` は素数、`mod > 1`、
`a % mod != 0` が必要。素数性自体は検査しない。

### `isqrt(n)`

$\lfloor\sqrt n\rfloor$ を誤差なく返す。`n < 0` では
`runtime_error` を送出する。

### `divisors(n)`

`n` の正の約数を昇順で返す。負数では絶対値を使い、`n == 0` では
`runtime_error` を送出する。

### `quotient_ranges(n)`

$\lfloor n/i\rfloor$ が等しい最大の半開区間 $[l,r)$ を `l` の昇順で返す。
対象は $1\le i\le n$。`n == 0` では空、`n < 0` では例外を送出する。

### `to_base_digits(n, base)`

非負整数 `n` を `base` 進数の桁列へ変換し、最上位桁から返す。
0は `{0}`。`base < 2` では例外を送出する。

### `from_base_digits(digits, base)`

最上位桁から並んだ桁列を非負整数へ戻す。空列は0。範囲外の桁、
`base < 2`、`unsigned long long` を超える結果では例外を送出する。

## 計算量の概要

- `to_base_digits`: $O(\log n)$
- `from_base_digits`: $O(|digits|)$
- `divisors`: $O(\sqrt n)$
- `quotient_ranges`: $O(\sqrt n)$
- `ext_gcd`, `gcd`, `lcm`, `inv_mod`: $O(\log \min(a, b))$
- `isqrt`: $O(1)$
- `pow_mod`, `inv_mod_prime`: $O(\log n)$

## API別の時間計算量・空間計算量

$D$ を桁数、$K$ を返す要素数とする。固定幅整数の算術を $O(1)$ と数える。

| API | 時間計算量 |
| --- | --- |
| `math::abs_ll(x)` | $O(1)$ |
| `math::gcd(a,b)` / `ext_gcd(a,b)` / `lcm(a,b)` | $O(\log\max(|a|,|b|))$ |
| `math::pow_mod(x,n,mod)` | $O(\log(n+1))$ |
| `math::inv_mod(a,mod)` | $O(\log\max(|a|,mod))$ |
| `math::inv_mod_prime(a,mod)` | $O(\log mod)$ |
| `math::isqrt(n)` | $O(1)$ |
| `math::divisors(n)` | $O(\sqrt{|n|}+K)$、出力 $O(K)$ |
| `math::quotient_ranges(n)` | $O(\sqrt n)$、出力 $O(K)$（$K=O(\sqrt n)$） |
| `math::to_base_digits(n,base)` | $O(D)$、出力 $O(D)$ |
| `math::from_base_digits(digits,base)` | $O(D)$ |

`ExtGcdResult` と `QuotientRange` の各field参照は $O(1)$。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
