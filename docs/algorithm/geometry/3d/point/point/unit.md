---
title: 3D Unit Vector (三次元単位ベクトル)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point/unit.hpp
---

## API

- `unit(point)`: `point` と同じ向きで長さ $1$ のベクトルを返す。先に最大絶対成分で割ってから長さを求めるため、元のノルムがオーバーフローする大きさでも処理できる。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 零ベクトル、および NaN や無限大を含む入力には `std::invalid_argument` を送出する。
- 零判定は許容誤差を使わない厳密な判定（全成分が $0$）であるため、非正規化数の成分を持つ非零ベクトルも正規化できる。
