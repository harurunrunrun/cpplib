---
title: 3D Closest Pair Result (三次元最近点対結果型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/closest_pair_3d_result.hpp
---

## API

- `ClosestPair3DResult::first`: 点対のうち小さい方のインデックス。
- `ClosestPair3DResult::second`: 点対のうち大きい方のインデックス。
- `ClosestPair3DResult::distance`: 2 点間のユークリッド距離。

## API別の時間計算量・空間計算量

- 構築・各メンバへのアクセス: 時間・追加領域 $O(1)$。

## 注意点

- 型自体はインデックスや距離を検査しない。`closest_pair_3d` が返す値では `first < second` が成り立つ。
