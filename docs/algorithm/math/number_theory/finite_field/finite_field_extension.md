---
title: Finite Field Extension Arithmetic (有限体拡大算術)
documentation_of: ../../../../../src/algorithm/math/number_theory/finite_field/finite_field_extension.hpp
---

素体 $\mathbb F_p$ 上のmonic多項式 $f$ による商 $\mathbb F_p[x]/(f)$ の算術を提供する。

## `FiniteFieldExtension<Prime>`

係数を低次数順の `vector<uint64_t>` で表す。有限体にするにはコンストラクタへ既約monic多項式を渡す。

### `degree()` / `modulus()`

拡大次数と法多項式を返す。時間計算量は $O(1)$。

### `normalize(value)`

係数をmod `Prime`にし、法多項式で剰余を取る。入力次数を $e$、拡大次数を $d$ とすると時間計算量 $O(ed)$、空間計算量 $O(e)$。

### `zero()` / `one()`

加法・乗法単位元を返す。時間・空間計算量は $O(d)$。

### `add(first, second)` / `subtract(first, second)`

加減算を返す。時間・空間計算量は $O(d)$。

### `multiply(first, second)`

積を法多項式で簡約して返す。時間計算量 $O(d^2)$、空間計算量 $O(d)$。

### `power(base, exponent)`

二分累乗。時間計算量 $O(d^2\log exponent)$、空間計算量 $O(d)$。

### `inverse(value)` / `divide(numerator, denominator)`

多項式拡張Euclid互除法で逆元を求める。時間計算量 $O(d^2)$ 回の体演算、空間計算量 $O(d)$。

## 注意点

`Prime` が素数で、法多項式が既約であることは呼び出し側の前提。法多項式がmonicでない場合、または非可逆元を反転した場合は `std::invalid_argument` を送出する。乗算は `unsigned __int128` を用い、64-bit係数積のoverflowを避ける。
