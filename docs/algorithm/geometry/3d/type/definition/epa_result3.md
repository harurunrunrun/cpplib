---
title: EPA Result 3D (3次元EPA結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/epa_result3.hpp
---

## API

- `EPAResult3::intersects`: 入力の 2 形状が接触または交差するとき `true`。
- `penetration_depth`: 貫通深さ。第 1 の形状を `-normal * penetration_depth` だけ動かすと 2 形状を分離できる。
- `normal`: 第 1 の形状から第 2 の形状へ向かう接触単位法線。
- `point_on_first`, `point_on_second`: 終了時の面の重心座標から復元した接触点。
- `iterations`, `converged`: EPA の反復回数と、収束したかどうか。

## API別の時間計算量・空間計算量

- 構築・各メンバの参照: 時間・追加領域 $O(1)$。

## 注意点

- 接触領域が面や辺に広がる場合、返される代表点は一意でない。
- 貫通深さと接触点は入力座標系での値である。交差する場合の `normal` は有限な単位ベクトルである。
