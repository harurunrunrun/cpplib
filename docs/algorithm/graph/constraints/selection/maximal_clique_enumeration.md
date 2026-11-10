---
title: Maximal Clique Enumeration (極大クリーク列挙)
documentation_of: ../../../../../src/algorithm/graph/constraints/selection/maximal_clique_enumeration.hpp
---

無向グラフの極大クリークを重複なくすべて列挙する。

## API

```cpp
std::vector<std::vector<int>> enumerate_maximal_cliques(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
)
```

各極大クリークを昇順の頂点列で表し、クリークの列全体を辞書順に並べて返す。多重辺は 1 本にまとめ、自己ループは無視する。

## 時間計算量

頂点数を $N$、辺数を $M$、極大クリークの個数を $C$、出力されるクリークの頂点数の総和を $L$、$W = 64$ とする。

- 隣接ビット集合の構築: $O(N \lceil N/W \rceil + M)$
- ピボット付き Bron–Kerbosch 法: 再帰呼び出しの回数は最悪 $O(3^{N/3})$ で、各呼び出しでピボット選択に $O(N \lceil N/W \rceil)$ かかる。
- 出力の整列: 各クリーク内のソートに $O(L \log N)$、クリーク列全体の辞書順ソートに $O(N C \log (C+1))$

## 空間計算量

出力を除いて $O(N \lceil N/W \rceil)$（隣接ビット集合と、深さ $N$ 以下の再帰で保持する候補集合）、出力に $O(L)$。

## 注意点

- `vertex_count` が負、または辺の端点が $[0, N)$ の範囲外なら `runtime_error` を送出する。
- 0 頂点のグラフでは、空集合を唯一の極大クリークとして返す。
- 極大クリークは「これ以上頂点を追加できない」クリークであり、頂点数が最大のクリークだけを意味するわけではない。
