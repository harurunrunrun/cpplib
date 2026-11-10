---
title: Fast Polynomial Composition and Projection (高速合成・冪射影・逆関数)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_composition_fast.hpp
---

素数 `MOD` を法とする `math::Polynomial<MOD>` を扱う。`MOD` と NTT の長さの条件は `montgomery_ntt.hpp` と同じである。

## API

| API | 結果 |
| --- | --- |
| `polynomial_compose_fast(f, g, n)` | $f(g(x)) \bmod x^n$。$g_0 \ne 0$ でもよい |
| `polynomial_power_projection(f, w, k, m)` | $i = 0, \ldots, m-1$ に対する $[x^k]\, w(x) f(x)^i$ |
| `polynomial_reversion_fast(f, n)` | $f(g(x)) \equiv x \pmod{x^n}$ となる $g$（逆関数） |

いずれも `math::` 名前空間の関数テンプレートである。

合成は、2 変数の母関数 $w(x)/(1 - y f(x))$ に対する Bostan–Mori 法を転置して求める。$g_0 \ne 0$ の場合は先に `outer` を $g_0$ だけ Taylor シフトする。逆関数は冪射影に Lagrange の反転公式を適用し、形式的冪級数の指数関数・対数関数で復元する。

`polynomial_power_projection` で $f_0 \ne 0$ の場合も、最後に分母の逆元を掛けて正しい値を返す。重み付きの係数 $[x^K]\, w(x) g(x)^i$ を一括で列挙する用途や、外側の多項式を変えながら何度も合成する処理の双対としても使える。

## 時間計算量・空間計算量

入力の長さ・出力の長さ・射影の次数がいずれも $O(N)$ のとき、各操作の時間は $O(M(N) \log N) = O(N \log^2 N)$。合成の再帰の中間領域は $O(N \log N)$、冪射影は $O(N)$。中間の積には 2 変数の係数を 1 列に並べたものと、必要な区間だけを取り出す巡回 middle product を使う。

## 注意点

- `polynomial_reversion_fast(f, n)` は $f_0 = 0$、$f_1 \ne 0$、$n < \mathrm{MOD}$ を要求し、違反時は `std::invalid_argument` を送出する（$n = 0$ では空、$n = 1$ では $\lbrace 0 \rbrace$ を返す）。
- middle product の変換長が $\mathrm{MOD} - 1$ を割り切らない場合は `std::length_error` を送出する。
- GCC 13 / C++20 を想定する。
