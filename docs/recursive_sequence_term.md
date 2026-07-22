---
title: Recursive Sequence Term (線形漸化式の指定項) [SEQ]
documentation_of: ../src/algorithm/math/sequence/recursive_sequence_term.hpp
---

係数が一定の線形漸化式について、1始まりの指定項を法付きで求める。

## `recursive_sequence_term`

```cpp
uint64_t recursive_sequence_term(
    const vector<uint64_t>& initial,
    const vector<uint64_t>& coefficient,
    uint64_t one_based_index
);

uint64_t recursive_sequence_term(
    const vector<uint64_t>& initial,
    const vector<uint64_t>& coefficient,
    uint64_t one_based_index,
    uint64_t modulus
);
```

`initial[i] = a_(i+1)`、`coefficient[j]` は直前から `j+1` 個前の項に掛かる係数を表す。
3引数版の法は $10^9$ であり、高速畳み込みを使う。
4引数版は任意の64-bit法を受け取り、二乗時間の多項式剰余を使う。

## `recursive_sequence_term_static_modulus`

```cpp
template<int MOD>
uint64_t recursive_sequence_term_static_modulus(
    const vector<uint64_t>& initial,
    const vector<uint64_t>& coefficient,
    uint64_t one_based_index
);
```

コンパイル時定数 `MOD` を法として同じ項を求める。
Bostan--Mori 法と任意法畳み込みを使うため、高次数向けである。

## API別の時間計算量・空間計算量

次数を $K$、求める添字を $N$、次数 $K$ の多項式乗算時間を $M(K)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `recursive_sequence_term(initial, coefficient, N)` | $O(M(K)\log N)$ | $O(K)$ |
| `recursive_sequence_term(initial, coefficient, N, modulus)` | $O(K^2\log N)$ | $O(K)$ |
| `recursive_sequence_term_static_modulus<MOD>` | $O(M(K)\log N)$ | $O(K)$ |

## 注意点

- 初期値と係数は同じ非0長で、添字と法は正でなければならない。
- `MOD` は正の `int` の範囲で指定する。
- 任意の64-bit法が必要な場合は4引数版を使う。
