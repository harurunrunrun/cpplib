---
title: Is Undirected Tree (無向グラフの木判定) [PT07Y]
documentation_of: ../src/algorithm/graph/traversal/is_undirected_tree.hpp
---

無向グラフが木であるかを判定する。

## is_undirected_tree

```cpp
bool is_undirected_tree(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

頂点数が正で、グラフが連結かつ閉路を持たない場合に限り `true` を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、辺数を $M$、逆 Ackermann 関数を $\alpha$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `is_undirected_tree` | $O(M\alpha(N))$ | $O(N)$ |

## 注意点

- 頂点番号は `0` 以上 `vertex_count` 未満でなければならない。
- `vertex_count` が負、または頂点番号が範囲外の場合は `runtime_error` を送出する。
- 頂点数 `0` のグラフは木ではないものとして `false` を返す。
