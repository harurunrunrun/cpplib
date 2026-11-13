---
title: Evolutionary Box Constraints (進化計算の箱型制約補助)
documentation_of: ../../../../src/approximate/evolutionary/detail/box_constraints.hpp
---

進化計算で、個体を箱型制約に対して検査・射影する内部 API である。以下、次元を $D$ とする。

## API

### `validate_box(lower, upper, dimension)`

境界の次元、有限性、大小関係を検査する。時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `clamp_to_box(point, lower, upper)`

各座標を閉区間 `[lower[i], upper[i]]` に収め、`NaN` は下限に置き換える。時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `validate_population_box(population, lower, upper, minimum_population)`

個体数、次元の一貫性、境界、全座標の有限性と範囲を検査する。個体数を $N$ として、時間計算量は $O(ND)$、追加空間計算量は $O(1)$ である。

## 注意点

- `approximate::evolutionary::detail` の内部 API である。
- 不正な個体数・次元・境界・座標では `std::invalid_argument` を送出する。
