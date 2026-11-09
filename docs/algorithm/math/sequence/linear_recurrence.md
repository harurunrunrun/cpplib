---
title: Linear Recurrence (線形漸化式) [find_linear_recurrence] [kth_term_of_linearly_recurrent_sequence]
documentation_of: ../../../../src/algorithm/math/sequence/linear_recurrence.hpp
---

線形漸化式

$$
a_n=\sum_{i=0}^{d-1}c_i a_{n-1-i}
$$

を扱う。既知の漸化式から項を求める操作は可換環上で動作する。

テンプレート引数 `MAX_DEGREE` を省略する（デフォルト値 `-1`）と、実行時のサイズに合わせた `std::vector` を使う可変長版になる。`MAX_DEGREE` を明示すると、容量が固定された `std::array` を使う固定長版になる。

```cpp
LinearRecurrence<Mint> dynamic(initial, coefficients);
LinearRecurrence<Mint, 1000> fixed(initial, coefficients);
auto inferred = berlekamp_massey(sequence);
```

## LinearRecurrence

```cpp
LinearRecurrence<T, MAX_DEGREE> recurrence(initial, coefficient);
auto value = recurrence.kth_term(k);
```

`initial[i]` は $a_i$、`coefficient[i]` は $c_i$ であり、両者の長さは一致させる。次数 $0$ は零列を表す。

- デフォルトコンストラクタは次数 $0$ の零列を作る。
- ポインタ版コンストラクタ `LinearRecurrence(initial, coefficient, d)` は `initial[0..d)` と `coefficient[0..d)` をコピーする。$d \ge 0$ かつ（固定長版では）$d \le \texttt{MAX_DEGREE}$ が必要で、$d > 0$ で null ポインタを渡すと `std::runtime_error` を送出する。
- `std::vector` 版コンストラクタは、2 つの列の長さが等しいことと、容量内であることを検査し、違反時は `std::runtime_error` を送出する。
- `degree()` は次数 $d$ を返す。
- `initial_value(i)` は $a_i$ を、`coefficient(i)` は $a_{n-1-i}$ に掛ける $c_i$ を返す。$0 \le i < d$ が必要で、範囲外なら `std::runtime_error` を送出する。
- `kth_term(k)` は 0-indexed の $a_k$ を Bostan–Mori 法（素朴な多項式乗算）で返す。$k < d$ では初期値をそのまま返し、次数 $0$ では $0$ を返す。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| コンストラクタ | $O(d)$ | 固定長版 $O(1)$、可変長版 $O(d)$ |
| `degree()` | $O(1)$ | $O(1)$ |
| `initial_value(i)` | $O(1)$ | $O(1)$ |
| `coefficient(i)` | $O(1)$ | $O(1)$ |
| `kth_term(k)` | $O(d^2\log(k+1))$ | 固定長版 $O(\texttt{MAX_DEGREE})$、可変長版 $O(d)$ |

## Berlekamp–Massey

```cpp
auto recurrence = berlekamp_massey<T, MAX_DEGREE>(sequence);
auto recurrence2 = berlekamp_massey<T, MAX_DEGREE>(pointer, n);
```

与えられた長さ $N$ の列を生成する最小次数の線形漸化式を求め、`LinearRecurrence` として返す。除算を使うため、値型は体でなければならない。
ポインタ版は null ポインタ（$n > 0$ のとき）と負の長さを検査し、固定長版では必要な次数が容量を超える場合も検査する。違反時は `std::runtime_error` を送出する。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `berlekamp_massey(sequence)` | $O(Nd)$、最悪 $O(N^2)$ | 固定長版 $O(\texttt{MAX_DEGREE})$、可変長版 $O(N)$ |

## 畳み込みを用いる Bostan–Mori

```cpp
auto value = bostan_mori(initial, coefficient, k);
```

`std::vector<Modint<MOD>>` に対する高速版で、多項式乗算に `arbitrary_mod_convolution` を使う。`MOD` が $998244353$ などの NTT 向きの法なら NTT による畳み込みを直接使い、それ以外の法では 3 つの NTT 向き素数による畳み込みと中国剰余定理で計算する。
$M(d)$ を次数 $d$ の多項式乗算の時間とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `bostan_mori(initial, coefficient, k)` | $O(M(d)\log(k+1))$ | $O(d)$ |

`k` は 0-indexed である。2 つの列の長さが異なる場合は `std::runtime_error` を送出し、次数 $0$ では $0$ を返す。
