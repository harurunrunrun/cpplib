---
title: Minimum Foxling Crackers (狐を呼ぶ最小クラッカー数) [FOXLINGS]
documentation_of: ../src/algorithm/graph/minimum_foxling_crackers.hpp
---

狐の友達関係から、全員を呼び寄せるために必要なクラッカー数の最小値を返す。

# minimum_foxling_crackers

```cpp
long long minimum_foxling_crackers(
    long long foxling_count,
    const vector<pair<long long, long long>>& friendships
);
```

`foxling_count` 頂点の無向グラフについて、連結成分数を返す。辺に現れない頂点を明示的に保持しないため、頂点数が非常に大きい場合にも利用できる。

## API別の時間計算量・空間計算量

友達関係の数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_foxling_crackers` | $O(M \log M)$ | $O(M)$ |

## 注意点

- 頂点番号は `0` 以上 `foxling_count` 未満でなければならない。
- `foxling_count` が負、または頂点番号が範囲外の場合は `runtime_error` を送出する。
- 自己ループと多重辺は答えに影響しない。
