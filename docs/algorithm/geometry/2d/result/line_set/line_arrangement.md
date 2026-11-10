---
title: Line Arrangement (直線アレンジメント)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/line_set/line_arrangement.hpp
---

無限直線の集合が作る平面グラフの 1 次元骨格（頂点と辺）を構築する。

## API

`LineArrangementResult line_arrangement(const std::vector<Line>& lines)` は次のメンバを持つ結果を返す。

- `vertices`: 近い点を同一視した交点の列（$(x, y)$ の辞書順）。
- `bounded_edges`: 同じ直線上で隣り合う交点を結ぶ辺（`first_vertex`, `second_vertex`, `line`）。
- `rays`: 交点を持つ各直線の両端にある半直線（始点の交点番号 `vertex`、単位方向ベクトル `direction`, `line`）。
- `unbounded_lines`: 交点を 1 つも持たない直線（`origin`, 単位方向ベクトル `direction`, `line`）。

各要素の `line` は元の直線の番号である。

## 時間計算量

直線の本数を $N$、交点の記録数を $K = O(N^2)$ として、時間 $O(N^2 + K \log K)$、空間 $O(K + N)$。

## 注意点

- 面（フェイス）の列挙は行わず、1 次元骨格全体を返す。
- 平行な直線の組と一致する直線の組は交点を作らない。
- 2 点が近すぎる退化直線と非有限な座標には `std::invalid_argument` を送出する。
