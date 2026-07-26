---
title: 3D Point Deduplication Default Method (三次元点重複除去の既定法)
documentation_of: ../../../../src/algorithm/geometry/3d/deduplicate_points_3d_default.hpp
---

## API

- `deduplicate_points_3d(points, relative_tolerance, absolute_tolerance)`: 入力順を保ち、既に残した代表点と三座標が許容誤差内で一致する点を除く。両誤差0ではexact sort、相対誤差0では固定幅cell、相対誤差正では代表点走査を使う。

## API別の時間計算量・空間計算量

点数を $N$、代表点数を $U$ とする。両誤差0は $O(N\log N)$ 時間・$O(N)$ 領域。相対誤差0かつ絶対誤差正は $O(N\log U)$ 時間・$O(U)$ 領域。相対誤差正は $O(NU)$ 時間・$O(U)$ 領域。

## 注意点

全座標と許容誤差は有限で、許容誤差は非負でなければならない。誤差付き一致は推移的でないため結果は入力順に依存する。
