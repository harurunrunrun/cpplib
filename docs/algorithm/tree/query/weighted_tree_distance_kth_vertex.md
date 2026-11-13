---
title: Weighted Tree Distance and K-th Vertex (重み付き木の距離・K 番目頂点) [QTREE2]
documentation_of: ../../../../src/algorithm/tree/query/weighted_tree_distance_kth_vertex.hpp
---

辺に重みを持つ静的な木に対し、パスの重みの和と、パス上の頂点を求める。
内部では `StaticTreePathFold<AddMonoid<Weight>{}, MAX_SIZE>`（ダブリング）を使うため、クエリの結果は根の選び方に依存しない。

## API

### `WeightedTreeDistanceKthVertex<Weight, MAX_SIZE>(vertex_count)`

```cpp
template<class Weight, int MAX_SIZE = -1>
struct WeightedTreeDistanceKthVertex;
explicit WeightedTreeDistanceKthVertex(int vertex_count);
```

頂点数 `vertex_count` の空の構造を作る。頂点番号は `[0, vertex_count)` である。
`MAX_SIZE` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` を使う。明示すると固定容量の配列を使う。

```cpp
WeightedTreeDistanceKthVertex<long long> dynamic(vertex_count);
WeightedTreeDistanceKthVertex<long long, 100000> fixed(vertex_count);
```

### `size()`

頂点数を返す。

### `add_edge(left, right, weight)`

重み `weight` の無向辺を追加する。`build` より前に呼ぶ。

### `build(root = 0)`

追加済みの辺が成す木を、`root` を根として前処理する。

### `distance(left, right)`

`left` から `right` への単純パスに含まれる辺の重みの総和を返す。
同じ頂点どうしでは加法の単位元を返す。

### `kth_vertex(left, right, order)`

`left` を 0 番目として、`left` から `right` への単純パス上の `order` 番目の頂点を返す。端点も数える。

## 時間計算量

$N$ を `vertex_count` とする。

- 構築: $O(N \log N)$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `build`: $O(N \log N)$
- `distance`: $O(\log N)$
- `kth_vertex`: $O(\log N)$

使用メモリは $O(N \log N)$ である。

## 注意点

- `0 <= vertex_count` が必要で、固定容量版では `vertex_count <= MAX_SIZE` も必要である。
- `Weight` は `AddMonoid<Weight>` で扱える型で、重みの和が `Weight` に収まる必要がある。
- `vertex_count > 0` では、辺数が `vertex_count - 1` の連結な無向木を渡す。
- `order` は `[0, パスの辺数]` の範囲でなければならない。
- 頂点・`order` の範囲違反、木でない入力、構築前のクエリ、構築後の辺の追加では `std::runtime_error` を送出する。
