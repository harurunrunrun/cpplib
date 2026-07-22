---
title: Manhattan MST (マンハッタン最小全域木) [manhattanmst]
documentation_of: ../src/algorithm/graph/spanning_tree/manhattan_mst.hpp
---

2次元点集合のマンハッタン距離に対する最小全域木を求める。

## 関数

```cpp
manhattan_mst<T>(points)
```

`points` は `ManhattanMstPoint<T>{x, y}` の列。

`ManhattanMstResult<T>` は次を持つ。

```cpp
T cost;
vector<ManhattanMstEdge<T>> edges;
```

## 時間計算量

- `manhattan_mst`: $O(N\log(N+1))$

4方向のsweep、$O(N)$ 本の候補辺整列、DSU処理、最大 $N-1$ 本の辺出力を含む。

## 空間計算量

- 候補辺、DSU、戻り値を含めて $O(N)$

## 注意点

点数が1以下ならcost 0・空辺列、それ以外は入力index間の辺を $N-1$ 本返す。座標変換・距離・総和が `T` に収まることを前提とし、本APIはrange例外を送出しない。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
