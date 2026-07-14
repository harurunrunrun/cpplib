---
title: Minimum Opinion Disagreement (意見不一致最小化) [COCONUTS]
documentation_of: ../src/algorithm/graph/minimum_opinion_disagreement.hpp
---

二値の投票を行う人々について、信念に反する投票数と友人間の不一致数の和の最小値を返す。

# minimum_opinion_disagreement

```cpp
int minimum_opinion_disagreement(
    const vector<bool>& beliefs,
    const vector<pair<int, int>>& friendships
);
```

`beliefs[v]` は人 `v` の信念、`friendships` は無向の友人関係を表す。各人の投票を自由に選んだときの、信念と異なる投票をした人数と異なる投票をした友人組数の合計の最小値を返す。

## API別の時間計算量・空間計算量

$N=beliefs.size()$、$M=friendships.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_opinion_disagreement` | $O(N^2(N+M))$ | $O(N+M)$ |

## 注意点

- 友人関係の端点は $[0,N)$ の相異なる頂点でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 同一の友人組を重複入力すると、それぞれを独立した不一致コストとして数える。
