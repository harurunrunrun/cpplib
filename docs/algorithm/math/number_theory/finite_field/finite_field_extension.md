---
title: Finite Field Extension Arithmetic (有限体拡大算術)
documentation_of: ../../../../../src/algorithm/math/number_theory/finite_field/finite_field_extension.hpp
---

素体 $\mathbb F_p$ 上の monic 多項式 $f$ による剰余環 $\mathbb F_p[x]/(f)$ の算術を提供する。$f$ が既約なら、これは $p^d$ 元体（$d = \deg f$）になる。

## `number_theory::FiniteFieldExtension<Prime>`

元の型 `Element` は `std::vector<std::uint64_t>` で、係数を低次の項から順に並べる。正規化された元は長さがちょうど $d$ である。以下、拡大次数を $d$ とする。

### コンストラクタ `FiniteFieldExtension(modulus)`

法多項式 `modulus`（係数は低次から）を受け取り、係数を mod `Prime` で正規化する。有限体として使うには既約な monic 多項式を渡す。

### `degree()` / `modulus()`

拡大次数 $d$ と法多項式を返す。時間計算量は $O(1)$。

### `normalize(value)`

各係数を mod `Prime` にとり、法多項式で割った余りを長さ $d$ の `Element` として返す。入力の長さを $e$ とすると、時間計算量 $O(ed)$、空間計算量 $O(e)$。

### `zero()` / `one()`

加法単位元・乗法単位元を返す。時間・空間計算量は $O(d)$。

### `add(first, second)` / `subtract(first, second)`

和・差を返す。正規化済みの入力に対して時間・空間計算量は $O(d)$。

### `multiply(first, second)`

積を法多項式で簡約して返す。時間計算量 $O(d^2)$、空間計算量 $O(d)$。

### `power(base, exponent)`

二分累乗法で $\mathrm{base}^{\mathrm{exponent}}$ を求める。時間計算量 $O(d^2 \log \mathrm{exponent})$、空間計算量 $O(d)$。

### `inverse(value)` / `divide(numerator, denominator)`

多項式の拡張ユークリッド互除法で逆元を求める。`divide` は `numerator` に `denominator` の逆元を掛ける。時間計算量は体の演算 $O(d^2)$ 回（各除算で係数の逆元を求める $O(\log p)$ が加わる）、空間計算量 $O(d)$。

## 注意点

- `Prime` が素数であること、法多項式が既約であることは呼び出し側の前提であり、検査しない。
- 法多項式が次数 $1$ 以上の monic 多項式でない場合はコンストラクタが `std::invalid_argument` を送出する。
- $0$ や法多項式と互いに素でない元を `inverse` / `divide` に渡すと `std::invalid_argument` を送出する。
- 係数の積には `unsigned __int128` を用いるので、64 bit 係数同士の積でもオーバーフローしない。
