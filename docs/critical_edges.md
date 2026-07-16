---
title: Critical Edges (切断辺列挙) [EC_P]
documentation_of: ../src/algorithm/graph/critical_edges.hpp
---

## critical_edges

```cpp
vector<pair<int, int>> critical_edges(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

## 引数

- `vertex_count`: 頂点数。
- `edges`: 0-indexed端点を持つ無向辺列。

## 戻り値

切断辺を、小さい端点を先にしたpairの辞書順で返す。

## API別の時間計算量・空間計算量

$N=vertex_count$、$M=edges.size()$、切断辺数を $B$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `critical_edges` | $O(N+M+B\log(B+1))$ | $O(N+M)$ |

## 注意点

- `vertex_count` は非負、端点は $[0,N)$ でなければならない。
- 前提違反時は `runtime_error` を送出する。並列辺は別の辺として扱う。
