---
title: Recursive Sequence Term (線形漸化式の指定項) [SEQ]
documentation_of: ../../../../src/algorithm/math/sequence/recursive_sequence_term.hpp
---

係数が一定の線形漸化式について、1-indexed で指定した項を、指定した法で求める。

## `math::recursive_sequence_term`

```cpp
std::uint64_t math::recursive_sequence_term(
    const std::vector<std::uint64_t>& initial,
    const std::vector<std::uint64_t>& coefficient,
    std::uint64_t one_based_index
);

std::uint64_t math::recursive_sequence_term(
    const std::vector<std::uint64_t>& initial,
    const std::vector<std::uint64_t>& coefficient,
    std::uint64_t one_based_index,
    std::uint64_t modulus
);
```

`initial[i]` は $a_{i+1}$ を表し、`coefficient[j]` $= c_j$ は $j+1$ 個前の項に掛かる係数を表す。すなわち $a_n = \sum_{j} c_j a_{n-1-j}$ である。
3 引数版の法は $10^9$ で、`recursive_sequence_term_static_modulus<1'000'000'000>` を呼んで高速な畳み込みを使う。
4 引数版は任意の 64 ビットの法を受け取り、二乗時間の多項式剰余（Kitamasa 法）を使う。

## `math::recursive_sequence_term_static_modulus`

```cpp
template<int MOD>
std::uint64_t math::recursive_sequence_term_static_modulus(
    const std::vector<std::uint64_t>& initial,
    const std::vector<std::uint64_t>& coefficient,
    std::uint64_t one_based_index
);
```

コンパイル時定数 `MOD` を法として同じ項を求める。
Bostan–Mori 法と任意法の畳み込み（`arbitrary_mod_convolution`）を使うため、次数が大きい場合に向く。

## API別の時間計算量・空間計算量

次数を $K$、求める添字を $N$、次数 $K$ の多項式乗算の時間を $M(K)$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `recursive_sequence_term(initial, coefficient, N)` | $O(M(K)\log N)$ | $O(K)$ |
| `recursive_sequence_term(initial, coefficient, N, modulus)` | $O(K^2\log N)$ | $O(K)$ |
| `recursive_sequence_term_static_modulus<MOD>` | $O(M(K)\log N)$ | $O(K)$ |

## 注意点

- 初期値と係数は同じ長さで、長さは $1$ 以上でなければならない。添字と法は正でなければならない。違反時は `std::invalid_argument` を送出する。
- `MOD` は正の `int` の範囲で指定する。
- 任意の 64 ビットの法が必要な場合は 4 引数版を使う。
