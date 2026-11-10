---
title: Translation-Only Iterative Closest Point (平行移動限定の反復最近点法)
documentation_of: ../../../src/approximate/geometry/iterative_closest_point.hpp
---

各点を最も近い対象点に対応付け、対応点の差の平均で平行移動を更新する、平行移動のみのモデルの ICP である。次元は `std::array<T, Dimension>` のテンプレート引数で指定する。回転や拡大縮小は推定しない。以下、`source` の点数を $N$、`target` の点数を $M$、次元を $D$、反復回数を $I$ とする。

## `brute_force_translation_icp<Dimension>(source, target, max_iterations = 100, tolerance = 1e-12L, initial_translation = {})`

`TranslationIcpResult` を返す。結果は、平行移動ベクトル `translation`、移動後の点 `transformed_points`、対応先の添字 `correspondences`、平均二乗誤差 `mean_squared_error`、実行した反復回数 `iterations`、収束したかを表す `converged` を持つ。対応点の探索は全探索で、距離が等しい場合は `target` 側の小さい添字を選ぶ。

- 1 反復の時間計算量: $O(NMD)$
- 全体の時間計算量: $O(INMD)$
- 追加空間計算量: $O((N+M)D)$

更新量の二乗が `tolerance` の二乗以下になるか、目的値の改善量が `tolerance` 以下になると停止する。

## 注意点

- ICP は局所的な手法であり、大域最適解は保証しない。最近傍の対応が切り替わるため、結果は初期の平行移動に依存する。
- 空の `source` に対しては、収束済みの空の結果を返す。`source` が空でないときに `target` が空なら `std::invalid_argument` を送出する。
- 非有限の座標や初期平行移動、負または非有限の `tolerance`、`max_iterations == 0` では `std::invalid_argument`、計算途中のオーバーフローでは `std::overflow_error` を送出する。
