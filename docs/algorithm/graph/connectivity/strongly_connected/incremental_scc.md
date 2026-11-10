---
title: Incremental Strongly Connected Components (増分強連結成分)
documentation_of: ../../../../../src/algorithm/graph/connectivity/strongly_connected/incremental_scc.hpp
---

空の有向グラフへ辺を順に追加していくとき、各辺について、その両端が初めて同じ強連結成分に入る時刻をオフラインで求める。時刻区間を分割統治し、中央の時刻での強連結成分で後半の頂点を縮約する。

## マージ時刻

```cpp
vector<int> incremental_scc(
    int vertex_count,
    const vector<pair<int, int>>& added_edges
)
```

`added_edges[i]` を時刻 $i + 1$ に追加する。戻り値の `merge_time[i]` は、時刻 $i + 1$ 以降で辺 $i$ の両端が同じ強連結成分に属する最初の時刻である。最後まで同じ強連結成分にならない場合は $M + 1$ になる。自己ループと多重辺を許す。

各時刻 $t$ について、`merge_time[i] == t` となる辺の両端を DSU でマージしていけば、辺を $t$ 本追加した時点での強連結成分による分割が得られる。

## 時間計算量

頂点数を $N$、辺数を $M$ とする。

- $O((N + M) \log (M + 1))$

分割統治の各階層で扱う頂点数は辺の端点数の定数倍以下であり、強連結成分は CSR 形式の隣接リスト上で線形時間に求める。

## 空間計算量

- $O(N + M)$

強連結成分の探索は再帰を使わない（分割統治の再帰の深さは $O(\log M)$）。

## 注意点

- `vertex_count` が負、または辺の端点が範囲外の場合は `std::runtime_error` を送出する。
- 頂点数が `INT_MAX` に等しい、または辺数が `INT_MAX` 以上の場合は、内部で使う $+1$ の番兵を表せないため `std::length_error` を送出する。
