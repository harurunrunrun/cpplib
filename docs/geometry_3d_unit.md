---
title: 3D Unit Vector (3次元単位ベクトル)
documentation_of: ../src/algorithm/geometry/3d/unit.hpp
---

## API

- `unit(point)`: `point` と同じ向きで長さが 1 のベクトルを返す。最大絶対成分で先に正規化するため、元のノルムがオーバーフローする大きさでも処理できる。

## API別の時間計算量・空間計算量

- `unit(point)`: 時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- ゼロベクトル、NaN、無限大を含む入力には `std::invalid_argument` を送出する。
- 判定は絶対許容誤差ではなく厳密なゼロ成分判定を使うため、subnormal の非ゼロベクトルも正規化できる。
