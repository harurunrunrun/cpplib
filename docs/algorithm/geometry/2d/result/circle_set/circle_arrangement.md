---
title: Circle Arrangement (円アレンジメント)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/circle_set/circle_arrangement.hpp
---

円の集合について、円同士の交点と、各円周を交点で分割した反時計回りの円弧を列挙する。

## API

`CircleArrangementResult circle_arrangement(const std::vector<Circle>& circles)` は次のメンバを持つ結果を返す。

- `vertices`: 近い点を同一視した交点の列（$(x, y)$ の辞書順）。
- `arcs`: `CircleArrangementArc` の列。各要素は元の円の番号 `circle`、始点・終点の交点番号 `first_vertex`, `second_vertex`、始点・終点の偏角 `first_angle`, `second_angle`（反時計回り、`second_angle` は必要に応じて $2\pi$ を加えた値）、`full_circle` を持つ。

交点を持たない円は、`full_circle = true`（交点番号は `static_cast<std::size_t>(-1)`、角度は $-\pi$ から $\pi$）の円弧を 1 本だけ持つ。交点を 1 個だけ持つ円は、その交点から一周して戻る円弧を 1 本持つ。

## 時間計算量

円の個数を $N$、交点の記録数を $K = O(N^2)$ として、時間 $O(N^2 + K \log K)$、空間 $O(K)$。

## 注意点

- 各円は有限な中心と、正の有限な半径を持たなければならない。違反時は `std::invalid_argument` を送出する。
- 一致する円の組は有限なセル分割を持たないため、`std::invalid_argument` を送出する。
