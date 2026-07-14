---
title: Count Dominance Pairs (支配関係を満たす順序対の数え上げ) [ABC231_F]
documentation_of: ../src/algorithm/range/count_dominance_pairs.hpp
---

2列の大小関係を同時に満たす添字の順序対を数える。

## 関数

```cpp
template<class A, class B>
long long count_dominance_pairs(
    const vector<A>& first,
    const vector<B>& second
)
```

`first[i] >= first[j]`かつ`second[i] <= second[j]`を満たす
順序対`(i, j)`の個数を返す。`i == j`も数える。

## API別の時間計算量・空間計算量

$N=first.size()$とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_dominance_pairs(first, second)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `first.size() == second.size()`でなければならない。
- `A`と`B`はコピー可能な全順序型で、比較演算を提供する必要がある。
- 結果が`long long`に収まる長さでなければならない。
- 長さまたは結果型の前提違反では`runtime_error`を送出する。
