---
title: Incremental Strongly Connected Components (増分強連結成分) [incremental_scc]
documentation_of: ../src/algorithm/graph/incremental_scc.hpp
---

空の有向グラフへ辺を順に追加するとき、追加した各辺について、その時点以降に両端が初めて同じstrongly connected componentへ入る時刻をofflineで求める。時刻区間をdivide and conquerし、中央時刻のSCCで後半を縮約する。

# merge時刻

```cpp
vector<int> incremental_scc(
    int vertex_count,
    const vector<pair<int, int>>& added_edges
)
```

`added_edges[i]` を時刻 `i + 1` に追加する。返り値 `merge_time[i]` は、時刻 `i + 1` 以降で辺 `i` の両端が同じSCCに属する最初の時刻である。最後まで同じSCCにならない場合は `M + 1`。自己loopと多重辺を許す。

時刻 `t` ごとに `merge_time[i] == t` となる辺の両端をDSUでmergeすると、辺を `t` 本追加した時点のSCC partitionが得られる。

## 時間計算量

- `O((N+M) log(M+1))`

各divide-and-conquer階層で扱う頂点数は辺端点数の定数倍以下であり、SCCはCSR上で線形時間に求める。

## 空間計算量

- `O(N+M)`

SCC探索は再帰を使わない。

## 注意点

`vertex_count`または辺端点が範囲外の場合は `runtime_error` を送出する。
