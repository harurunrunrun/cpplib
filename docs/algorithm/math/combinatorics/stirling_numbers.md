---
title: Stirling Numbers (Stirling数) [stirling_number_of_the_first_kind] [stirling_number_of_the_first_kind_fixed_k] [stirling_number_of_the_second_kind] [stirling_number_of_the_second_kind_fixed_k]
documentation_of: ../../../../src/algorithm/math/combinatorics/stirling_numbers.hpp
---

第一種・第二種 Stirling 数の行（$n$ 固定）と列（$k$ 固定）を素数 `MOD` を法として求める。以下、$N$ は `order` または `maximum_order`、$M(N)$ は長さ $N$ の多項式積の計算量を表す。

## `math::signed_stirling_numbers_first_kind`

```cpp
template<int MOD>
std::vector<Modint<MOD>> math::signed_stirling_numbers_first_kind(
    std::size_t order
)
```

下降階乗

$$
x(x-1)\cdots(x-n+1)=\sum_{k=0}^{n}s(n,k)x^k
$$

の係数 $s(n,0),\ldots,s(n,n)$ を返す（$n$ は `order`）。上昇階乗 $x(x+1)\cdots(x+n-1)$ を、前半を Taylor シフトして掛け合わせる倍々の再帰で構成し、符号を変換する。

- 時間計算量: $O(M(N))$。NTT を用いるので $O(N \log N)$
- 空間計算量: $O(N)$

## `math::stirling_numbers_second_kind`

```cpp
template<int MOD>
std::vector<Modint<MOD>> math::stirling_numbers_second_kind(
    std::size_t order
)
```

恒等式

$$
x^n=\sum_{k=0}^{n}S(n,k)x(x-1)\cdots(x-k+1)
$$

で定義される第二種 Stirling 数 $S(n,0),\ldots,S(n,n)$ を返す。公式

$$
S(n,k)=\sum_{i=0}^{k}\frac{i^n}{i!}\frac{(-1)^{k-i}}{(k-i)!}
$$

を 1 回の畳み込みで評価する。

- 時間計算量: $O(M(N)+N\log N)$。NTT を用いるので $O(N\log N)$
- 空間計算量: $O(N)$

## `math::signed_stirling_numbers_first_kind_fixed_k`

```cpp
template<int MOD>
std::vector<Modint<MOD>>
math::signed_stirling_numbers_first_kind_fixed_k(
    std::size_t maximum_order,
    std::size_t fixed_cycles
)
```

`result[n] = s(n, fixed_cycles)` となる長さ `maximum_order + 1` の列を返す。指数型母関数 $(\log(1+x))^k/k!$ を形式的冪級数の冪（`fps_pow`）で計算する。

- 時間計算量: $O(M(N) + N \log \mathrm{MOD})$。現在の FPS 実装（Newton 法による $\log$・$\exp$）では $O(N\log N)$
- 空間計算量: $O(N)$

## `math::stirling_numbers_second_kind_fixed_k`

```cpp
template<int MOD>
std::vector<Modint<MOD>>
math::stirling_numbers_second_kind_fixed_k(
    std::size_t maximum_order,
    std::size_t fixed_parts
)
```

`result[n] = S(n, fixed_parts)` となる長さ `maximum_order + 1` の列を返す。指数型母関数 $(e^x-1)^k/k!$ を形式的冪級数の冪で計算する。

- 時間計算量: $O(M(N))$。現在の FPS 実装では $O(N\log N)$
- 空間計算量: $O(N)$

## 注意点

- いずれの API も `order < MOD`（`maximum_order < MOD`）が必要で、満たさなければ `length_error` を送出する。
- 法は素数で、必要な 2 の冪の NTT 長が `MOD - 1` を割り切ることを想定する。
- 列を求める 2 つの API は、固定値（`fixed_cycles` / `fixed_parts`）が `maximum_order` を超えると `invalid_argument` を送出する。
