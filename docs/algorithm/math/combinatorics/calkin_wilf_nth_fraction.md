---
title: Calkin-Wilf Nth Fraction (Calkin-Wilf木の第N分数) [NG0FRCTN]
documentation_of: ../../../../src/algorithm/math/combinatorics/calkin_wilf_nth_fraction.hpp
---

Calkin–Wilf 木の頂点を幅優先順に番号付けし、指定した位置の既約な正の分数を返す。

## `calkin_wilf_nth_fraction`

```cpp
pair<uint64_t, uint64_t> calkin_wilf_nth_fraction(
    uint64_t one_based_index
);
```

根を $1/1$ とし、$i/j$ の左の子を $i/(i+j)$、右の子を $(i+j)/j$ とした木の `one_based_index` 番目（1-indexed）の分数を `{numerator, denominator}` で返す。

## 時間計算量・空間計算量

`one_based_index` を $n$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `calkin_wilf_nth_fraction` | $O(\log n)$ | $O(1)$ |

## 注意点

- `one_based_index >= 1` が必要で、違反時は `invalid_argument` を送出する。
- 分子・分母が `uint64_t` を超える場合の検査は行わない（$n < 2^{64}$ の範囲では分子・分母は Fibonacci 数で抑えられ、$F_{65} < 2^{64}$ なので収まる）。
