---
title: 2D Point (二次元点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/point.hpp
---

`long double` 座標の点またはベクトルを表す。

## API

- `Point()`: 原点 $(0, 0)$ を構築する。
- `Point{x, y}`: 座標を指定して構築する。
- `+point`, `-point`, `point + other`, `point - other`: 単項プラス・符号反転とベクトルの加減算。
- `point * scalar`, `scalar * point`, `point / scalar`: スカラー倍・スカラー除算。
- `+=`, `-=`, `*=`, `/=`: 複合代入。
- `operator<`: 座標の厳密な辞書順比較。
- `operator==`: 各座標の差を `geometry_sign` で判定する、許容誤差付きの等値比較。

## 計算量

すべて時間 $O(1)$、追加空間 $O(1)$。

## 注意点

`operator==` と `operator<` は異なる等値基準を使う。除数が $0$ でないことは呼び出し側が保証する。
