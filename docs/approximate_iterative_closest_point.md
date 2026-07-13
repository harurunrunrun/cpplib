---
title: Translation-Only Iterative Closest Point (平行移動限定の反復最近点法)
documentation_of: ../src/approximate/geometry/iterative_closest_point.hpp
---

# Iterative Closest Point（平行移動モデル）

各点を最寄りの対象点へ対応付け、対応点差の平均で平行移動を更新する。次元は `std::array<T, Dimension>` のテンプレート引数で指定する。回転・拡大縮小は推定しない。

## `brute_force_translation_icp<Dimension>(source, target, max_iterations, tolerance, initial_translation)`

`TranslationIcpResult` を返す。結果には `translation`、`transformed_points`、`correspondences`、`mean_squared_error`、実行した `iterations`、`converged` が入る。対応点探索は全探索で、距離が等しい場合は対象側の小さい添字を選ぶ。

- 1反復の時間計算量: $O(NMD)$
- 全体の時間計算量: $O(INMD)$
- 追加空間計算量: $O((N+M)D)$
- 近似・収束: ICPは局所法であり大域最適解を保証しない。更新量の二乗が `tolerance^2` 以下、または目的値の改善が `tolerance` 以下なら停止する。最近傍対応が切り替わるため、初期平行移動に依存する。

空の `source` は収束済みの空結果を返す。空の `target` は空でない `source` に対して使用できない。非有限座標、次元のoverflow、0回反復は例外とする。
