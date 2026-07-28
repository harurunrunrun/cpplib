---
title: 2D Point (二次元点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/point.hpp
---

`long double` 座標の点またはベクトルを表す。

## API

- `Point{x, y}`: 座標を指定して構築する。
- `+point`, `-point`, `point + other`, `point - other`: 符号反転とベクトル加減算。
- `point * scalar`, `scalar * point`, `point / scalar`: スカラー演算。
- `+=`, `-=`, `*=`, `/=`: 複合代入。
- `operator<`: 座標の厳密な辞書順比較。
- `operator==`: 許容誤差付き等値比較。

## API別の時間計算量・空間計算量

- すべて時間 $O(1)$、追加領域 $O(1)$。

## 注意点

`operator==` と `operator<` は異なる等値基準を使う。除数が0でないことは呼び出し側が保証する。
