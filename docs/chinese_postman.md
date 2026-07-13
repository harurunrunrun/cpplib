---
title: Undirected Chinese Postman
documentation_of: ../src/algorithm/graph/chinese_postman.hpp
---

非負重み無向グラフの全辺を少なくとも1回通る閉じたwalkの最小距離を求める。

## 辺と結果

```cpp
ChinesePostmanEdge<T>{from, to, cost}

struct ChinesePostmanResult<T> {
    T cost;
    vector<pair<int, int>> paired_odd_vertices;
};
```

`cost` が最小距離。`paired_odd_vertices` は、重複して通る最短路で結んだ奇数次数頂点のpair。自己loopと多重辺を許す。

## `undirected_chinese_postman`

```cpp
template<signed_integral T>
optional<ChinesePostmanResult<T>> undirected_chinese_postman(
    int vertex_count,
    const vector<ChinesePostmanEdge<T>>& edges
)
```

全辺の重み和へ、奇数次数頂点間の最短距離に対する最小重み完全マッチングの重みを加える。全点対最短路はFloyd--Warshall、完全マッチングはweighted blossomで求める。

次数が正の頂点が複数の連結成分に分かれている場合、1つのclosed walkでは全辺を通れないため `nullopt`。孤立頂点は通るべき辺を持たないので無視する。辺が0本なら距離0。

頂点数、端点、負の辺重みが不正なら `invalid_argument`。最短路または答えが `T` に収まらない場合は `overflow_error`。`T` はbool以外の符号付き整数型とする。

## 計算量

$V$ を頂点数、$E$ を辺数、$K$ を奇数次数頂点数とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `undirected_chinese_postman` | $O(V^3+K^3+E)$ | $O(V^2+K^2+E)$ |

返すpair数は $K/2$。実際に重複する辺列が必要な場合は、各pair間の最短路を別途復元する。
