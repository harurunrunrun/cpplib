---
title: Weighted Undirected Edge Type and Input Rules (重み付き無向辺型と入力規則)
documentation_of: ../src/approximate/graph/weighted_undirected_graph.hpp
---

重み付き無向グラフを受け取る近似アルゴリズムで共通に使う辺型である。

## API

### `WeightedUndirectedEdge`

~~~cpp
struct WeightedUndirectedEdge {
    size_t left;
    size_t right;
    long double weight;
};
~~~

構築と各memberの参照は時間計算量・追加空間計算量は $O(1)$。

## 注意点

端点は `[0, vertex_count)`、重みは有限な非負値でなければならない。自己辺と重み0の辺は全APIで無視する。平行辺と逆向きに記述した同じ無向辺は重みを加算する。加算結果を `long double` で表せない場合は `std::overflow_error` を送出する。

端点範囲外には `std::out_of_range`、負・NaN・無限大の重みには `std::invalid_argument` を送出する。空グラフを許す。
