---
title: Unweighted Tree Radius (重みなし木の半径) [GCPC11J]
documentation_of: ../src/algorithm/tree/unweighted_tree_radius.hpp
---

重みなし無向木について、各頂点から最遠頂点までの距離の最小値である木の半径を返す。

# unweighted_tree_radius

```cpp
int unweighted_tree_radius(
    int vertex_count,
    const vector<pair<int, int>>& edges
);
```

木の直径を $D$ とすると、戻り値は $\lceil D/2\rceil$ である。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `unweighted_tree_radius` | $O(N)$ | $O(N)$ |

## 注意点

- `vertex_count == 0` では0を返す。
- 各端点は $[0,N)$ で、辺集合は連結な木でなければならない。違反時は `runtime_error` を送出する。
- 距離は辺数で測る。
