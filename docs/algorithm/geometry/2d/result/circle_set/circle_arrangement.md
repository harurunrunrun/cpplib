---
title: Circle Arrangement (円アレンジメント)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/circle_set/circle_arrangement.hpp
---

円集合の交点と、各円周を交点間に分けた反時計回りのarcを列挙する。

## API

`CircleArrangementResult circle_arrangement(circles)` は `vertices` と `arcs` を返す。
交点がない円は `full_circle=true` のarcを1本持つ。

## 時間計算量

For N circles and K = O(N^2) intersection records: O(N^2 + K log K) time and O(K) space.

## 注意点

正の有限半径が必要。一致円は有限なcell分割を持たないため拒否する。
