---
title: Neighboring Town Pairs (隣接する町の組) [ROADNET]
documentation_of: ../src/algorithm/graph/neighboring_town_pairs.hpp
---

全頂点対最短距離表から、ほかの町を経由する最短路を持たない町の組を列挙する。

# neighboring_town_pairs

```cpp
vector<pair<int, int>> neighboring_town_pairs(
    const vector<vector<long long>>& distances
);
```

距離表に対し、相異なる `left`, `right` の間に
`distances[left][right] == distances[left][middle] + distances[middle][right]`
を満たす第三の町 `middle` が存在しない組を、辞書順で1回ずつ返す。

## API別の時間計算量・空間計算量

町の数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `neighboring_town_pairs` | $O(N^3)$ | 戻り値を除き $O(1)$ |

## 注意点

- 距離表は正方、非負、対称で、対角成分が `0` でなければならない。
- 入力検査に違反する場合は `runtime_error` を送出する。
- 距離和の比較は符号付き128 bitで行う。
