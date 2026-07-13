---
title: 2D RANSAC Line Fitting (2次元RANSAC直線推定)
documentation_of: ../src/approximate/geometry/ransac_line.hpp
---

# RANSACによる直線推定

2点を無作為抽出して直線候補を作り、外れ値を含む2次元点群から支持点の多い直線を選ぶ。垂直線も方向ベクトルで表すため特別扱いしない。

## `ransac_line(points, inlier_distance, iterations, random, minimum_inliers)`

距離が `inlier_distance` 以下の点を支持点とする。支持点数を最大化し、同数なら支持点の距離二乗和を最小化する。結果は `RansacLineResult` で、`found`、直線上の `point`、単位 `direction`、`inliers`、`squared_error` を持つ。

- 時間計算量: $O(IN)$（$I$ は `iterations`）
- 追加空間計算量: $O(N)$
- 確率的性質: 真の支持点率を $w$ とすると、独立な2点がともに支持点となる確率は概ね $w^2$。少なくとも一度その組を引く確率は $1-(1-w^2)^I$ である。ただし重複点の組は候補にならない。

乱数エンジンは参照で渡し、同じ実装・初期状態なら同じ結果になる。非有限座標、負の閾値、0回反復、距離計算のoverflowは例外とする。全点が同一点の場合は `found == false` となる。

## 注意点

入力座標と許容誤差は各APIの有限性・次元条件を満たし、距離・面積の中間値も表現可能でなければならない。乱択手法の結果は乱数器状態に依存する。
