---
title: Maximum Potholer Count (洞窟同時踏破人数) [POTHOLE]
documentation_of: ../src/algorithm/graph/maximum_potholer_count.hpp
---

最上部と最下部に接続する通路だけを共有できない有向洞窟で、同時に踏破できる人数を返す。

## maximum_potholer_count

```cpp
int maximum_potholer_count(
    int chamber_count,
    const vector<pair<int, int>>& directed_corridors
);
```

部屋 `0` を最上部、部屋 `chamber_count-1` を最下部とする。各人が異なる最上部の出口と異なる最下部の入口を使い、中間通路は共有できるときの最大人数を返す。

## API別の時間計算量・空間計算量

$N=chamber_count$、$M=directed_corridors.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_potholer_count` | $O(N^2M)$ | $O(N+M)$ |

## 注意点

- `chamber_count` は2以上、通路の端点は $[0,N)$ でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 通路は有向であり、同じ向きの多重通路を別々に数える。
