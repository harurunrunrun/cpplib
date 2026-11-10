---
title: Manhattan MST (マンハッタン最小全域木) [manhattanmst]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/manhattan_mst.hpp
---

2 次元の点集合について、マンハッタン距離を辺の重みとする完全グラフの最小全域木を求める。

## 関数

```cpp
template <class T>
ManhattanMstResult<T> manhattan_mst(std::vector<ManhattanMstPoint<T>> points)
```

`points` は `ManhattanMstPoint<T>{x, y}` の列である。

戻り値 `ManhattanMstResult<T>` は次のメンバをもつ。

```cpp
T cost;
std::vector<ManhattanMstEdge<T>> edges;
```

`edges` の各要素 `ManhattanMstEdge<T>{from, to, cost}` は、入力での点の添字の間の辺とその距離を表す。

## 時間計算量

点の個数を $N$ として $O(N\log(N + 1))$。

4 方向の走査（`std::map` を使う）、$O(N)$ 本の候補辺のソート、Union-Find の処理、最大 $N-1$ 本の辺の出力を含む。

## 空間計算量

候補辺、Union-Find、戻り値を含めて $O(N)$。

## 注意点

- 点の個数が 1 以下ならコスト $0$ と空の辺列を返し、それ以外は $N-1$ 本の辺を返す。
- 座標の符号反転、距離、総和が `T` に収まることを前提とする（`std::abs` を使うため `T` は符号付きの型でなければならない）。この API は範囲検査による例外を送出しない。
