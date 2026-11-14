---
title: Basic Number Theory (初等整数論) [ALDS1_1_B] [NTL_1_B]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/basic_number_theory.hpp
---

整数まわりの基本関数をまとめたヘッダ。すべて名前空間 `math` にあり、引数と戻り値は主に `long long` である。

## 関数一覧

- `math::to_base_digits(n, base)`: `n` を `base` 進数の桁列に変換する。上位桁から並ぶ。
- `math::from_base_digits(digits, base)`: `base` 進数の桁列を整数に戻す。
- `math::divisors(n)`: `n` の正の約数を昇順で返す。
- `math::quotient_ranges(n)`: $\lfloor n/i \rfloor$ が等しい半開区間 $[l, r)$ を列挙する。
- `math::ext_gcd(a, b)`: $ax + by = \gcd(a, b)$ となる `{g, x, y}` を返す。
- `math::gcd(a, b)`: 最大公約数を返す。
- `math::lcm(a, b)`: 最小公倍数を返す。
- `math::abs_ll(x)`: 絶対値を返す。
- `math::isqrt(n)`: $\lfloor \sqrt{n} \rfloor$ を返す。
- `math::pow_mod(x, n, mod)`: $x^n \bmod \mathrm{mod}$ を返す。
- `math::inv_mod(a, mod)`: 法が素数でなくてもよい逆元を返す。
- `math::inv_mod_prime(a, mod)`: 法が素数の場合の逆元を返す。

## 戻り値型

### `ExtGcdResult`

- `g`: 非負の $\gcd(a,b)$。
- `x`, `y`: 元の符号を含めて $ax+by=g$ を満たす係数。
- `ext_gcd(0, 0)` では 3 つのメンバがすべて 0。

### `QuotientRange`

- `l`, `r`: 半開区間 $[l,r)$。
- `value`: 区間内のすべての $i$ に対する $\lfloor n/i\rfloor$。

## API 詳細

以下、例外はすべて `runtime_error` である。

### `abs_ll(x)`

`x` の絶対値を返す。`x == LLONG_MIN` のときは絶対値が `long long` に収まらないため例外を送出する。

### `gcd(a, b)` / `lcm(a, b)`

符号を無視した最大公約数・最小公倍数を返す。`gcd(0, 0)` と、どちらかが 0 の `lcm` は 0 である。`lcm` の乗算のオーバーフローは検査しない。

### `ext_gcd(a, b)`

`ExtGcdResult` を返す。`g` は非負で、$ax+by=g$ を満たす。

### `pow_mod(x, n, mod)`

$x^n\bmod \mathrm{mod}$ を $[0,\mathrm{mod})$ の範囲で返す。`n >= 0`、`mod > 0` が必要で、満たさない場合は `runtime_error` を送出する。

### `inv_mod(a, mod)`

$a$ の法 `mod` における乗法逆元を $[0,\mathrm{mod})$ の範囲で返す。`mod > 0` かつ $\gcd(a,\mathrm{mod})=1$ が必要で、逆元が存在しない場合は `runtime_error` を送出する。

### `inv_mod_prime(a, mod)`

Fermat の小定理で逆元を返す。`mod` は素数、`mod > 1`、`a % mod != 0` が必要。素数であること自体は検査しない。

### `isqrt(n)`

$\lfloor\sqrt n\rfloor$ を誤差なく返す（浮動小数点数の平方根を `__int128` で補正する）。`n < 0` では `runtime_error` を送出する。

### `divisors(n)`

`n` の正の約数を昇順で返す。負数では絶対値を使う。`n == 0` では `runtime_error` を送出する。

### `quotient_ranges(n)`

$1\le i\le n$ の範囲で、$\lfloor n/i\rfloor$ が等しい極大な半開区間 $[l,r)$ を `l` の昇順で返す。`n == 0` では空列を返し、`n < 0` では例外を送出する。

### `to_base_digits(n, base)`

非負整数 `n`（`unsigned long long`）を `base` 進数の桁列に変換し、最上位桁から返す。0 は `{0}` になる。`base < 2` では例外を送出する。

### `from_base_digits(digits, base)`

最上位桁から並んだ桁列を非負整数（`unsigned long long`）に戻す。空列は 0 になる。範囲外の桁、`base < 2`、`unsigned long long` を超える結果では例外を送出する。

## 時間計算量

$D$ を桁数、$K$ を返す要素数とする。固定幅整数の算術を $O(1)$ と数える。

| API | 時間計算量 |
| --- | --- |
| `math::abs_ll(x)` | $O(1)$ |
| `math::gcd(a,b)` / `ext_gcd(a,b)` / `lcm(a,b)` | $O(\log\max(\lvert a\rvert,\lvert b\rvert))$ |
| `math::pow_mod(x,n,mod)` | $O(\log(n+1))$ |
| `math::inv_mod(a,mod)` | $O(\log\max(\lvert a\rvert,\mathrm{mod}))$ |
| `math::inv_mod_prime(a,mod)` | $O(\log \mathrm{mod})$ |
| `math::isqrt(n)` | $O(1)$ |
| `math::divisors(n)` | $O(\sqrt{\lvert n\rvert}+K)$、出力 $O(K)$ |
| `math::quotient_ranges(n)` | $O(\sqrt n)$、出力 $O(K)$（$K=O(\sqrt n)$） |
| `math::to_base_digits(n,base)` | $O(D)$、出力 $O(D)$ |
| `math::from_base_digits(digits,base)` | $O(D)$ |

`ExtGcdResult` と `QuotientRange` の各メンバの参照は $O(1)$。

## 注意点

- `abs_ll(LLONG_MIN)` は例外を送出する。内部で `abs_ll` を使う `gcd`・`lcm`・`ext_gcd`・`divisors` も、引数に `LLONG_MIN` を含むと同様に例外を送出する。
- `lcm` は結果が `long long` に収まることを前提とし、オーバーフローを検査しない。
