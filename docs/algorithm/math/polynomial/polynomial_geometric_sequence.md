---
title: Polynomial Geometric Samples (等比点での多項式計算) [multipoint_evaluation_on_geometric_sequence] [polynomial_interpolation_on_geometric_sequence] [shift_of_sampling_points_of_polynomial]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_geometric_sequence.hpp
---

素数法 `MOD` の `math::Polynomial<MOD>` を扱う。NTT の長さが法で許容されることが必要。

## 時間計算量

| 関数 | 結果 | 時間 |
| --- | --- | --- |
| `polynomial_evaluate_geometric(f,a,r,m)` | `f(a*r^i)`, `0<=i<m` | $O(M(n+m))$ |
| `polynomial_interpolate_geometric(values,a,r)` | `a*r^i` で与えられた値を持つ次数 `n` 未満の多項式 | $O(M(n))$ |
| `polynomial_shift_samples(values,c,m)` | `f(0),...,f(n-1)` から `f(c),...,f(c+m-1)` | $O(M(n+m))$ |

$M(n)$ は多項式乗算時間。いずれも追加領域 $O(n+m)$。等比評価は chirp 変換、等比補間は積多項式の係数と Lagrange 重みを利用する。補間点は相異なる必要があり、`n<MOD`。`a=0`、`r=0`、`r^n=1` の有効な境界も処理する。標本点シフトは法をまたぐ場合と既知の標本点に重なる場合も処理する。

類題: 等比数列上で多項式の値をまとめて求め、その値から係数を復元する。シフト版では区間 `[L,R)` の連続標本を別の区間へ移せる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
