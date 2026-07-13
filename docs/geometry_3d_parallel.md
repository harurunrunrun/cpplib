---
title: 3D Parallelism (三次元平行判定)
documentation_of: ../src/algorithm/geometry/3d/parallel.hpp
---

## API

- `parallel(left, right)`: 2ベクトルが平行か許容誤差付きで判定する。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

有効な方向を表す非零ベクトルを与える。
