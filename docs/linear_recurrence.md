---
title: Linear Recurrence (線形漸化式) [kth_term_of_linearly_recurrent_sequence]
documentation_of: ../src/algorithm/math/linear_recurrence.hpp
---

線形漸化式

$$
a_n=\sum_{i=0}^{d-1}c_i a_{n-1-i}
$$

を扱う。既知の漸化式から項を求める操作は可換環上で動作する。

## LinearRecurrence

```cpp
LinearRecurrence<T, MAX_DEGREE> recurrence(initial, coefficient);
auto value = recurrence.kth_term(k);
```

`initial[i]` は $a_i$、`coefficient[i]` は $c_i$ であり、両者の長さは一致させる。
内部配列の容量は `MAX_DEGREE` で固定される。次数0は零列を表す。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| コンストラクタ | $O(d)$ | $O(1)$ |
| `degree()` | $O(1)$ | $O(1)$ |
| `initial_value(i)` | $O(1)$ | $O(1)$ |
| `coefficient(i)` | $O(1)$ | $O(1)$ |
| `kth_term(k)` | $O(d^2\log(k+1))$ | $O(\mathrm{MAX\_DEGREE})$ |

## Berlekamp--Massey

```cpp
auto recurrence = berlekamp_massey<T, MAX_DEGREE>(sequence);
```

与えられた列のprefixを生成する最小次数の線形漸化式を求める。除算を使うため、値型は体を成す必要がある。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `berlekamp_massey(sequence)` | $O(Nd)$、最悪 $O(N^2)$ | $O(\mathrm{MAX\_DEGREE})$ |

## NTT版 Bostan--Mori

```cpp
auto value = bostan_mori(initial, coefficient, k);
```

`Modint<MOD>` のvectorに対する高速版。`MOD = 998244353` などNTT可能な法では既存のNTT畳み込みを使う。
$M(d)$ を次数 $d$ の多項式乗算時間とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `bostan_mori(initial, coefficient, k)` | $O(M(d)\log(k+1))$ | $O(d)$ |

次数0は零列を表す。`k` は0-indexed。
## 注意点

- default constructorは次数0の零列。`degree()` は次数 $d$ を返す。
- pointer constructorは `initial[0..d)` と `coefficient[0..d)` をコピーする。
  $0\le d\le MAX_DEGREE$ が必要で、$d>0$ のnull pointerは例外。
- vector constructorは両vectorの長さ一致と容量内であることを検査する。
- `initial_value(i)` は $a_i$、`coefficient(i)` は
  $a_n$ に掛ける $c_i$ を返す。$0\le i<d$ が必要。
- `kth_term(k)` は0-indexedの $a_k$。$k<d$ では初期値をそのまま返す。
- pointer版 `berlekamp_massey(sequence,n)` はnull、負の長さ、必要次数の
  容量超過を検査する。Berlekamp--Masseyだけは除算を使うため値型が体である必要がある。
- 高速 `bostan_mori(initial,coefficient,k)` は2vectorの長さが異なると例外。
  次数0では0を返す。
