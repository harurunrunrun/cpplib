---
title: Weighted Undirected Edge Type and Input Rules (重み付き無向辺型と入力規則)
documentation_of: ../../../src/approximate/graph/weighted_undirected_graph.hpp
---

重み付き無向グラフを受け取る近似アルゴリズムで共通に使う辺型と、その入力規則（Weighted Undirected Graph Input）である。

## API

### `WeightedUndirectedEdge`

~~~cpp
struct WeightedUndirectedEdge {
    size_t left;
    size_t right;
    long double weight;
};
~~~

構築と各メンバの参照は時間計算量・追加空間計算量とも $O(1)$ である。

## 注意点

- 端点は $[0, \texttt{vertex_count})$ になければならず、範囲外なら `std::out_of_range` を送出する
- 重みは有限な非負値でなければならず、負・NaN・無限大の重みには `std::invalid_argument` を送出する
- 自己ループと重み 0 の辺は全 API で無視する
- 平行辺や、逆向きに記述した同じ無向辺は重みを加算する。加算結果を `long double` で表せない場合は `std::overflow_error` を送出する
- 空グラフを許す
