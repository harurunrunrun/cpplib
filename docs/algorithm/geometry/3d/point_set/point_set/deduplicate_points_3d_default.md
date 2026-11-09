---
title: 3D Point Deduplication Default Method (三次元点重複除去の既定法)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point_set/point_set/deduplicate_points_3d_default.hpp
---

## API

- `deduplicate_points_3d(points, relative_tolerance = 0, absolute_tolerance = GEOMETRY3D_EPS)`: 入力順を保ち、既に残した代表点と三座標がすべて許容誤差内で一致する点を除いた列を返す。

許容誤差に応じて次のように手法を切り替える。

- 両誤差が $0$: 完全一致による重複除去（ソート）。
- 相対誤差が $0$ で絶対誤差が正: 幅 `absolute_tolerance` の固定幅セルと近傍セルの探索。
- 相対誤差が正: 代表点の線形走査。

## API 別の時間計算量・空間計算量

点数を $N$、代表点数を $U$ とする。

- 両誤差が $0$: 時間 $O(N \log N)$、領域 $O(N)$。
- 相対誤差が $0$ かつ絶対誤差が正: 時間 $O(N \log U)$、領域 $O(U)$。
- 相対誤差が正: 時間 $O(NU)$、領域 $O(U)$。

## 注意点

- 全座標と許容誤差は有限で、許容誤差は非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 誤差付きの一致は推移的でないため、結果は入力順に依存する。
- 相対誤差が正のときに $O(N \log^3 N)$ で処理したい場合は `deduplicate_points_3d_relative_range_tree` を使う。
