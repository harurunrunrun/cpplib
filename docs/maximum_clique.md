---
title: Maximum Clique (最大クリーク) [ABC002_4]
documentation_of: ../src/algorithm/graph/maximum_clique.hpp
---

無向グラフの最大クリークを1つ求める。

## 関数

```cpp
maximum_clique(graph)
```

`graph[v]` に隣接頂点を入れる。無向グラフである必要がある。

戻り値は最大クリークの頂点列。

## 時間計算量

$N$ を頂点数、$M$ を隣接リストの総要素数、$C$ を返すクリークの大きさとする。

- 隣接行列の構築・無向性検査: $O(N^2+M)$
- `maximum_clique`: worst-case $O(2^N N^3+N^2+M)$、出力構築 $O(C)$

branch-and-boundにより実際の探索量は入力に依存するが、worst-caseは指数時間。

## 空間計算量

- 隣接行列と再帰中の候補列を含めて $O(N^2)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
