---
title: Stirling Numbers (Stirling数) [stirling_number_of_the_first_kind] [stirling_number_of_the_second_kind]
documentation_of: ../../../../src/algorithm/math/combinatorics/stirling_numbers.hpp
---

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

の係数 $s(n,0),\ldots,s(n,n)$ を返す。平行移動と積の分割統治により
上昇階乗多項式を構成し、符号を変換する。

- 時間計算量: $O(M(N)\log N)$。NTTを用いる場合は $O(N\log^2 N)$
- 空間計算量: $O(N\log N)$

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

で定義される第二種Stirling数 $S(n,0),\ldots,S(n,n)$ を返す。
公式

$$
S(n,k)=\sum_{i=0}^{k}\frac{i^n}{i!}\frac{(-1)^{k-i}}{(k-i)!}
$$

を1回の畳み込みで評価する。

- 時間計算量: $O(M(N)+N\log N)$。NTTを用いる場合は $O(N\log N)$
- 空間計算量: $O(N)$

## 注意点

両APIとも `order < MOD` が必要である。法は必要な階乗が可逆で、
必要な2冪NTT長が `MOD - 1` を割り切る素数を想定する。
