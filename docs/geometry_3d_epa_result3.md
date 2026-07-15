---
title: EPA Result 3D (3次元EPA結果)
documentation_of: ../src/algorithm/geometry/3d/epa_result3.hpp
---

## API

- `EPAResult3::intersects`: 入力形状が接触または交差するとき真。
- `penetration_depth`: 第1形状を `-normal * penetration_depth` だけ動かすと分離できる深さ。
- `normal`: 第1形状から第2形状を向く接触単位法線。
- `point_on_first`, `point_on_second`: 終了面の重心座標から復元した接触点。
- `iterations`, `converged`: EPA の反復回数と収束フラグ。

## API別の時間計算量・空間計算量

- 各構造体の構築・参照は $O(1)$。

## 注意点

接触点が面や辺に広がる場合、返る代表点は一意でない。
