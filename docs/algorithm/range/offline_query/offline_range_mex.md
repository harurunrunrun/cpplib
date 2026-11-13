---
title: Offline Range MEX (オフライン区間MEX)
documentation_of: ../../../../src/algorithm/range/offline_query/offline_range_mex.hpp
---

静的な列に対する区間 MEX クエリを、右端の昇順に処理する。各値の最終出現位置をセグメント木に保持し、その最小値で二分探索するため、永続版より空間が少ない。

## offline_range_mex

```cpp
vector<size_t> offline_range_mex(
    const vector<int64_t>& values,
    const vector<pair<size_t,size_t>>& ranges
)
```

各半開区間の答えを入力順に返す。空区間の答えは $0$ である。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `offline_range_mex` | $O((N+Q)\log N)$ | $O(N+Q)$ |

## 注意点

- `left > right` または `right > N` の区間では `out_of_range` を送出する。
- 負の値と $N$ より大きい値は MEX に影響しない。
