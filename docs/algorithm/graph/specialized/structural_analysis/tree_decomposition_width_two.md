---
title: Tree Decomposition of Width Two (木幅2以下の木分解) [tree_decomposition_width_2]
documentation_of: ../../../../../src/algorithm/graph/specialized/structural_analysis/tree_decomposition_width_two.hpp
---

単純無向グラフの木幅が 2 以下かを判定し、そうであれば幅 2 以下の木分解を構成する。
次数 2 以下の頂点を消去し、残った 2 つの隣接頂点の間に補填辺（fill edge）を加える、という消去順を利用する。

## `TreeDecompositionWidthTwoResult`

```cpp
struct TreeDecompositionWidthTwoResult {
    bool valid;
    vector<vector<int>> bags;
    vector<pair<int,int>> tree_edges;
    int width() const noexcept;
};
```

`valid` が `true` なら、`bags[i]` は分解木の節点 `i` のバッグ（昇順）で、`tree_edges` は節点間の辺である。非連結グラフでは各成分の分解木を任意の辺で連結し、1 つの木として返す。空グラフではバッグと辺は空である。

`width()` は最大のバッグサイズから 1 を引いた値を返す。空の有効な分解では `-1`、`valid == false` では `0` を返す。

- 時間計算量: `width()` は $O(K)$（$K$ はバッグ数）
- 追加空間計算量: $O(1)$

## `tree_decomposition_width_two`

```cpp
TreeDecompositionWidthTwoResult tree_decomposition_width_two(
    int vertex_count,
    const vector<pair<int,int>>& edges
);
```

頂点集合を `[0, vertex_count)` とするグラフを受け取る。木幅が 2 以下なら、次を満たす分解を返す。

- 各バッグの大きさは高々 3
- 元のグラフの各辺について、両端点をともに含むバッグがある
- 元のグラフの各頂点について、それを含むバッグの集合は分解木上で連結である

バッグは消去順に 1 頂点につき 1 個作られるので、有効な場合のバッグ数は頂点数に等しい。
木幅が 3 以上なら `valid == false` で、バッグと辺は空である。

- 時間計算量: $O((N+M)\log N)$
- 空間計算量: $O(N+M)$

$N$ は頂点数、$M$ は辺数である。重複辺は 1 本にまとめる。消去中に加える補填辺は高々 $N$ 本である。

## 注意点

- 負の頂点数、範囲外の端点、自己ループには `invalid_argument` を送出する。
- 多重辺は許すが、同じ無向辺として扱う。
- 返る分解は一意ではない。
