---
title: Unweighted Tree Radius (重みなし木の半径) [GCPC11J]
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/unweighted_tree_radius.hpp
---

重みなし無向木について、木の半径（各頂点から最も遠い頂点までの距離の最小値）を返す。

## API

```cpp
int unweighted_tree_radius(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
);
```

木の直径を $D$ とすると、戻り値は $\lceil D/2\rceil$ である。

## API 別の時間計算量・空間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `unweighted_tree_radius` | $O(N)$ | $O(N)$ |

## 注意点

- `vertex_count == 0` では `0` を返す。
- `vertex_count` は非負で、辺はちょうど $N-1$ 本、各端点は $[0, N)$ で、辺集合は連結な木でなければならない。違反時は `std::runtime_error` を送出する。
- 距離は辺数で測る。
