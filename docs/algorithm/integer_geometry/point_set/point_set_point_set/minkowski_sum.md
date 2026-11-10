---
title: Exact Minkowski Sum (厳密Minkowski和)
documentation_of: ../../../../../src/algorithm/integer_geometry/point_set/point_set_point_set/minkowski_sum.hpp
---

2 つの点集合、または 2 つの凸多角形の Minkowski 和の凸包を返す。

## API

- `minkowski_sum_convex(first, second)`: 凸多角形同士を、辺ベクトルのマージで加算する。入力は時計回り・反時計回りのどちらでもよい。結果は $y$ 座標最小（同じなら $x$ 座標最小）の点から反時計回りに並ぶ。どちらかが空なら空列を返す。
- `minkowski_sum(first, second)`: 一般の点集合をそれぞれ凸包にしてから Minkowski 和を返す。

## API別の時間計算量・空間計算量

- `minkowski_sum_convex(first, second)`: 時間 $O(n + m)$、戻り値を含む追加領域 $O(n + m)$
- `minkowski_sum(first, second)`: 時間 $O(n \log n + m \log m)$、戻り値を含む追加領域 $O(n + m)$
- $n, m$ はそれぞれの点数である。

## 注意点

- 計算は任意精度整数（`Integer`）と既約有理数（`Rational`）で厳密に行う。記載した計算量は算術演算の回数であり、実際にはこれに加えて座標のビット長に応じた多倍長整数演算のコストがかかる。
- すべての API は名前空間 `integer_geometry` に属する。
- `minkowski_sum_convex` の入力は凸多角形（巡回順に並んだ頂点列）でなければならない。
