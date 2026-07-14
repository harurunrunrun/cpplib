---
title: Numbers with Three Distinct Prime Factors (異なる3素因数を持つ数) [AMR11E]
documentation_of: ../src/algorithm/math/nth_three_prime_factors.hpp
---

異なる素因数を3個以上持つ正整数を昇順に前計算する。

# `ThreeDistinctPrimeFactorsTable`

```cpp
explicit ThreeDistinctPrimeFactorsTable(int maximum_rank);
```

先頭 `maximum_rank` 個を構築する。

# `size`

```cpp
int size() const;
```

保持する個数を返す。

# `value`

```cpp
int value(int one_based_rank) const;
```

1始まり順位の値を返す。

## API別の時間計算量・空間計算量

探索で到達した上限を `L` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(L\log\log L)$ | $O(L)$ |
| `size` | $O(1)$ | $O(1)$ |
| `value` | $O(1)$ | $O(1)$ |

## 注意点

- 順位が構築範囲外なら `out_of_range` を送出する。
