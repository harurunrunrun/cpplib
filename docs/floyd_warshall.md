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
vector<vector<char>> reachable;
vector<char> negative;
bool has_negative_cycle;
```

`reachable[i][j]` は `i` から `j` へ到達可能なら真。内部の到達判定は `inf` と分離されているため、間接経路の距離が `inf` 以上でも扱える。入力行列では値が `inf` の要素を辺なしとみなすため、重みがちょうど `inf` の直接辺は表現できない。

`path(from, to)` で復元できる。到達不能な場合と、到達可能な負閉路を経由して最短距離が定まらない場合は空配列を返す。

`negative[v]` は `v` が負閉路上にあることを表す。

距離と辺コストの加算結果は `T` で表現できる必要がある。

## 時間計算量

- $O(N^3)$
