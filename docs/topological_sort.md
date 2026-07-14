---
title: Topological Sort (トポロジカルソート) [GRL_4_B]
documentation_of: ../src/algorithm/graph/topological_sort.hpp
---

有向グラフをトポロジカルソートする。

# 関数

```cpp
topological_sort(graph)
```

`TopologicalSortResult` は次を持つ。

```cpp
vector<int> order;
bool is_dag;
```

閉路がある場合 `is_dag` は `false`。この場合、`order` は全頂点を含むが、
トポロジカル順であることを保証しない。

頂点番号の昇順に DFS を開始し、各隣接リストを格納順に走査する。返す `order`
は DFS の帰りがけ順を反転した決定的な順序になる。

## 時間計算量

- `topological_sort`: $O(N+M)$

全辺の行き先検査と DFS、最大 $N$ 頂点の `order` 出力を含む。

## 空間計算量

- 戻り値、訪問状態、反復 DFS の stack を含めて $O(N)$

## 注意点

`is_dag == false` のとき、`order` をトポロジカル順として使用してはならない。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
