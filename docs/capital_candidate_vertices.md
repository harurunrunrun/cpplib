---
title: Capital Candidate Vertices (首都候補頂点) [CAPCITY]
documentation_of: ../src/algorithm/graph/capital_candidate_vertices.hpp
---

# capital_candidate_vertices

```cpp
vector<int> capital_candidate_vertices(const vector<vector<int>>& graph)
```

## 引数

- `graph`: 0-indexed頂点の有向隣接リスト。

## 戻り値

すべての頂点から到達できる頂点を昇順で返す。該当頂点がなければ空vector。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、返す頂点数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `capital_candidate_vertices` | $O(N+M+K\log(K+1))$ | $O(N+M)$ |

## 注意点

- 各辺の行き先は $[0,N)$ でなければならない。違反時は `out_of_range` を送出する。
