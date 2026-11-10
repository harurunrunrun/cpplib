---
title: Exact Convexity Predicate (厳密凸性判定)
documentation_of: ../../../../../src/algorithm/integer_geometry/predicate/point_set/is_convex.hpp
---

巡回順に与えた多角形が凸かを判定する。

## API

- `bool is_convex(const std::vector<Point>& polygon, bool strict = false)`: 連続する 3 頂点の向きがすべて同じ（一直線を除く）なら `true` を返す。`strict` が `true` なら、一直線に並ぶ連続 3 頂点も許さない。頂点数が 3 未満、または全頂点が一直線上の場合は `false` を返す。

## API別の時間計算量・空間計算量

- `is_convex(polygon, strict)`: 時間 $O(n)$、追加領域 $O(1)$

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
- 自己交差の有無は検査しない。星形多角形のように、各頂点で同じ向きに曲がるが自己交差する多角形にも `true` を返す。
