---
title: Tree Decomposition of Width Two (木幅2以下の木分解)
documentation_of: ../src/algorithm/graph/specialized/tree_decomposition_width_two.hpp
---

単純無向グラフの木幅が2以下かを判定し、該当する場合は幅2以下の木分解を構成する。
次数2以下の頂点を消去し、残る2近傍の間にfill edgeを加える消去順を利用する。

## `TreeDecompositionWidthTwoResult`

```cpp
struct TreeDecompositionWidthTwoResult {
    bool valid;
    vector<vector<int>> bags;
    vector<pair<int,int>> tree_edges;
    int width() const;
};
```

`valid` が `true` なら、`bags[i]` は分解木の節点 `i` のバッグで、
`tree_edges` はその節点間の辺である。非連結グラフでは各成分の分解木を任意の辺で
連結し、一つの木として返す。空グラフではバッグと辺は空である。

`width()` は最大バッグサイズから1を引いた値を返す。空の有効な分解では `-1`、
`valid == false` では0を返す。

- 時間計算量: `width()` は $O(K)$
- 追加空間計算量: $O(1)$

$K$ はバッグ数。

## `tree_decomposition_width_two`

```cpp
TreeDecompositionWidthTwoResult tree_decomposition_width_two(
    int vertex_count,
    const vector<pair<int,int>>& edges
);
```

頂点が `[0, vertex_count)` のグラフを受け取る。木幅が2以下なら、次を満たす分解を返す。

- 各バッグの大きさは高々3
- 各元辺の両端を同時に含むバッグがある
- 各元頂点を含むバッグが分解木上で連結である

木幅が3以上なら `valid == false` で、バッグと辺は空。

- 時間計算量: $O((N+M)\log N)$
- 空間計算量: $O(N+M)$

重複辺は一つにまとめる。消去中に加えるfill edgeは高々 $N$ 本である。

## 注意点

負の頂点数、範囲外端点、自己ループには `invalid_argument` を送出する。
多重辺は許すが同じ無向辺として扱う。返る分解は一意ではない。
