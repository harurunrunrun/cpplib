---
title: Undirected Chinese Postman (無向中国人郵便配達問題) [DPL_2_B]
documentation_of: ../../../../../src/algorithm/graph/specialized/route_optimization/chinese_postman.hpp
---

非負重みの無向グラフで、全辺を少なくとも 1 回ずつ通る閉路（閉じた歩道）の最小距離を求める。

## 辺と結果

```cpp
ChinesePostmanEdge<T>{from, to, cost}

struct ChinesePostmanResult<T> {
    T cost;
    vector<pair<int, int>> paired_odd_vertices;
};
```

`cost` は最小距離である。`paired_odd_vertices` は、間の最短路を重複して通ることで結んだ奇数次数頂点の組の列である。自己ループと多重辺を許す。

## `undirected_chinese_postman`

```cpp
template<signed_integral T>
optional<ChinesePostmanResult<T>> undirected_chinese_postman(
    int vertex_count,
    const vector<ChinesePostmanEdge<T>>& edges
)
```

全辺の重みの和に、奇数次数頂点間の最短距離を重みとする最小重み完全マッチングの重みを加えたものを返す。Dijkstra 法は奇数次数頂点を始点とするものだけを実行し、完全マッチングは重み付き blossom 法で求める。全点対の距離表は構築しない。

次数が正の頂点が複数の連結成分に分かれている場合は、1 つの閉じた歩道で全辺を通れないため `nullopt` を返す。孤立頂点は通るべき辺を持たないので無視する。辺が 0 本なら距離は $0$ である。

頂点数・端点・辺重みが不正（頂点数が負、端点が範囲外、辺重みが負）なら `invalid_argument` を送出する。Dijkstra 法を用いるため辺重みは $0$ 以上に限り、負辺に対応する方式へ暗黙に切り替えることはしない。最短路の長さまたは答えが `T` に収まらない場合は `overflow_error` を送出する。`T` は符号付き整数型（`std::signed_integral`）とする。

## API別の時間計算量・空間計算量

$V$ を頂点数、$E$ を辺数、$K$ を奇数次数頂点の個数、$B(K)$ を $K$ 頂点の最小重み一般マッチングの計算量とする。現在の重み付き blossom 法では $B(K)=O(K^3)$ である。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `undirected_chinese_postman` | $O(E+V+K(E+V)\log V+B(K))$ | $O(V+E+K^2)$ |

返す組の個数は $K/2$ である。実際に重複して通る辺の列が必要な場合は、各組の間の最短路を別途復元する。

## 注意点

- 辺の端点は $[0,V)$ に収める必要がある。
- 多重辺と自己ループを入力できる。
