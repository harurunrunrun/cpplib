---
title: One Modular Power Root (素数法での冪根) [kth_root_mod]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/one_kth_root_mod_prime.hpp
---

## API

- `math::one_kth_root_mod_prime(value, exponent, prime)`: $x^{\mathrm{exponent}} \equiv \mathrm{value} \pmod{\mathrm{prime}}$ を満たす $x$ を 1 つ `std::optional<u64>` で返す。解がなければ `std::nullopt` を返す。
    - 法 `prime` は素数でなければならない。
    - `exponent == 0` も扱う。このとき $x^0 = 1$ とみなし、$\mathrm{value} \equiv 1$ なら $1$ を、そうでなければ `std::nullopt` を返す。
    - $\mathrm{value} \equiv 0$（かつ `exponent > 0`）なら $0$ を返す。

全解を列挙せず、乗法群全体での離散対数も使わないので、巨大な指数の根の存在判定や、任意の根を 1 つ求める用途に向く。全解が必要な場合は `modular_kth_roots_prime` を使う。

## アルゴリズム

$k$ を `exponent`、$p$ を `prime`、$g = \gcd(k, p-1)$ とする。まず $\mathrm{value}^{(p-1)/g} \equiv 1$ かで解の存在を判定する。$k/g$ の部分は逆元を指数に掛けて処理し、$g$ を Pollard–Rho 法で素因数分解して、各素因数冪 $q^e \parallel g$ について Sylow 部分群の中で位ごとに根を持ち上げる。$q^{e+1} \mid p-1$ のときに限り、位数 $q$ の部分群で $O(\sqrt q)$ の baby-step giant-step を（$p-1$ における $q$ の指数と $e$ の差の回数だけ）行う。

## 時間計算量

$g$ の素因数分解に加えて、上の条件を満たす各素因数 $q$ について期待 $O(\sqrt q + v_q \log p)$ 時間程度（$v_q$ は $p-1$ における $q$ の指数）、空間 $O(\sqrt q)$。

## 注意点

- `prime` が素数でない（Miller–Rabin で判定）場合は `std::invalid_argument` を送出する。
- GCC 13 / C++20 を想定する（`__int128` を使用）。
