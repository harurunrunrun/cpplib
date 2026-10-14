---
title: Fast Polynomial Composition and Projection (高速合成・冪射影・逆関数)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_composition_fast.hpp
---

素数体 `MOD` 上の `math::Polynomial<MOD>` を扱う。`MOD` とNTT長の条件は `montgomery_ntt.hpp` と同じ。

| API | 結果 |
| --- | --- |
| `polynomial_compose_fast(f,g,n)` | `f(g(x)) mod x^n`。`g[0]` が非零でも可 |
| `polynomial_power_projection(f,w,k,m)` | `i=0,...,m-1` に対する `[x^k] w(x)*f(x)^i` |
| `polynomial_reversion_fast(f,n)` | `f(g(x))=x mod x^n` となる `g` |

合成は、二変数母関数 `w(x)/(1-y*f(x))` の Bostan--Mori 法を転置して求める。逆関数は冪射影へ Lagrange 反転公式を適用し、形式的冪級数の指数・対数で復元する。`f[0]=0`、`f[1]!=0`、`n<MOD` が逆関数の条件。

## 時間計算量

入力長・出力長・射影次数がいずれも $O(N)$ のとき、各操作の時間は $O(M(N)\log N)=O(N\log^2 N)$。合成の再帰中間領域は $O(N\log N)$、冪射影は $O(N)$。中間積には二変数係数の平坦化と、必要区間だけを取り出す循環 middle product を使う。

類題: 任意の重み付き係数 `[x^K] w(x)*g(x)^i` を一括列挙する。合成と転置の関係を使うため、外側多項式を変えて何度も合成する処理の双対としても利用できる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
