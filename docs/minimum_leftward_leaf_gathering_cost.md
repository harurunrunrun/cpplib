---
title: Minimum Leftward Leaf Gathering Cost (葉の左向き収集最小費用) [NKLEAVES]
documentation_of: ../src/algorithm/other/minimum_leftward_leaf_gathering_cost.hpp
---

一直線上の重み付き要素を左向きに移動して指定個数の山へ集める最小費用を返す。

## minimum_leftward_leaf_gathering_cost

```cpp
long long minimum_leftward_leaf_gathering_cost(
    const vector<int>& weights, int pile_count
);
```

位置を0から順に並べた `weights` を、空でない連続な `pile_count` 個の山へ集める最小の「重み×移動距離」合計を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_leftward_leaf_gathering_cost` | $O(KN)$ | $O(N)$ |

## 注意点

- 全重みは正で、$1\le K\le N$ でなければならない。
- 違反時は `runtime_error` を送出する。
