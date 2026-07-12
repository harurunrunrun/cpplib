---
title: Floyd Warshall
documentation_of: ../src/algorithm/graph/floyd_warshall.hpp
---

全点対最短路。負辺を扱える。

# 関数

```cpp
floyd_warshall<T>(dist, inf)
```

`dist[i][j]` に辺 `i -> j` の重み、辺がないところに `inf` を入れる。

`FloydWarshallResult<T>` は次を持つ。

```cpp
vector<vector<T>> dist;
vector<vector<int>> next;
vector<char> negative;
bool has_negative_cycle;
```

`path(from, to)` で復元できる。到達不能なら空配列を返す。

## 時間計算量

- $O(N^3)$
