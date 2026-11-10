---
title: 2D RANSAC Line Fitting (2次元RANSAC直線推定)
documentation_of: ../../../src/approximate/geometry/ransac_line.hpp
---

外れ値を含む 2 次元の点群から、2 点を無作為に抽出して直線の候補を作り、支持点（inlier）の多い直線を選ぶ。垂直な直線も方向ベクトルで表すため、特別扱いしない。以下、点数を $N$、`iterations` を $I$ とする。

## `ransac_line(points, inlier_distance, iterations, random, minimum_inliers = 2)`

直線からの距離が `inlier_distance` 以下の点を支持点とする。支持点の個数を最大化し、同数なら支持点の距離の二乗和を最小化する。戻り値の `RansacLineResult` は、見つかったかを表す `found`、直線上の点 `point`、単位方向ベクトル `direction`、支持点の添字 `inliers`、距離の二乗和 `squared_error` を持つ。

- 時間計算量: $O(IN)$
- 追加空間計算量: $O(N)$

真の支持点の割合を $w$ とすると、独立に選んだ 2 点がともに支持点である確率はおよそ $w^2$ であり、$I$ 回の試行のうち少なくとも 1 回そのような組を引く確率は $1-(1-w^2)^I$ である。ただし同一点の組は候補にならない。

## 注意点

- 乱数エンジンは参照で渡す。同じ実装・初期状態なら同じ結果になる。
- 非有限の座標、負または非有限の閾値、`iterations == 0`、`minimum_inliers < 2` では `std::invalid_argument`、距離計算のオーバーフローでは `std::overflow_error` を送出する。
- 全ての点が同一点の場合は `found == false` となる。
