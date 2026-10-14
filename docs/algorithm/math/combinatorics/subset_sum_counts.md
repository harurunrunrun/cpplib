---
title: Subset and Bounded Partition Counts (部分和と個数制限付き分割の個数) [sharp_p_subset_sum]
documentation_of: ../../../../src/algorithm/math/combinatorics/subset_sum_counts.hpp
---

`subset_sum_counts<MOD>(weights, maximum)` は、各要素を区別して0回または1回選ぶ部分和の個数を返す。`bounded_partition_counts<MOD>(types, maximum)` では各 `(weight,count)` 型を0個から `count` 個まで選び、同じ型の個体は区別しない。返り値の第0要素も含める。生成関数の対数を調和級数型のループで求め、形式的冪級数の指数で復元する。

## 時間計算量

入力項数を $N$、最大和を $T$ として $O(N+T\log(T+1))$、空間 $O(T)$。

## 注意点

`MOD` は $T$ より大きい素数。NTTの変換長の制約を満たすこと。部分和版の重み0は選ぶ・選ばないの2通りとして扱う。個数制限版の重みは正、個数は `uint64_t`。範囲外の最大和・負の重みは `invalid_argument`。
