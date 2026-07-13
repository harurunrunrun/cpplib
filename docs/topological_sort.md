---
title: Topological Sort (トポロジカルソート)
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

閉路がある場合 `is_dag` は `false`。

## 時間計算量

- `topological_sort`: $O(N+M)$

全辺の入次数計算と走査、最大 $N$ 頂点の `order` 出力を含む。

## 空間計算量

- 戻り値、入次数、queueを含めて $O(N)$

## API契約・前提・例外

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
