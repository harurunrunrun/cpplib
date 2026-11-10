---
title: Numbers with Three Distinct Prime Factors (異なる3素因数を持つ数) [AMR11E]
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/nth_three_prime_factors.hpp
---

相異なる素因数を 3 個以上持つ正整数を、小さい順に前計算する。

## `ThreeDistinctPrimeFactorsTable`

```cpp
explicit math::ThreeDistinctPrimeFactorsTable(int maximum_rank);
```

条件を満たす整数のうち小さい方から `maximum_rank` 個を構築する。篩の上限を $\max(64, 4K)$（$K$ は `maximum_rank`） から始め、足りなければ 2 倍にしてやり直す。

## `size`

```cpp
int size() const;
```

保持している個数を返す。

## `value`

```cpp
int value(int one_based_rank) const;
```

1 始まりの順位 `one_based_rank` の値を返す。

## 時間計算量・空間計算量

探索で最終的に到達した篩の上限を $L$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(L \log \log L)$ | $O(L)$ |
| `size` | $O(1)$ | $O(1)$ |
| `value` | $O(1)$ | $O(1)$ |

## 注意点

- `maximum_rank` が負なら `std::invalid_argument` を送出する。篩の上限が $10^9$ 程度を超える必要がある場合は `std::overflow_error` を送出する。
- 順位が構築した範囲外なら `std::out_of_range` を送出する。
