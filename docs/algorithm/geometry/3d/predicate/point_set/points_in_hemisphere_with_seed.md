---
title: Hemisphere Containment with Seed (シード指定の半球包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point_set/points_in_hemisphere_with_seed.hpp
---

指定したシードで決まる制約順を用いて、原点から各点への方向をすべて含む閉半球が存在するかを判定する。

## API

- `points_in_hemisphere_with_seed(points, random_seed)`: 閉半球が存在すれば `true` を返す。`random_seed`（`std::uint64_t`）は Seidel 法で制約を挿入する順序だけを変える。

## API別の時間計算量・空間計算量

点数を $N$ とする。シードを入力と独立に一様ランダムに選べば期待時間 $O(N)$、最悪時間 $O(N^2)$、追加領域 $O(N)$。

## 注意点

- 各点は有限な非零ベクトルでなければならず、違反時は `std::invalid_argument` を送出する。
- 空集合には `true` を返す。原点から点までの距離は結果に影響しない。
- 境界上の方向も許す閉半球判定である。内積の判定には `long double` の機械イプシロンの 256 倍程度の許容誤差を用いる。
- 法線のいずれか 1 座標を $\pm 1$ に固定した 6 通りの 2 変数線形計画問題を、Seidel の乱択増分法で解いて判定する。
- シードは判定の意味と許容誤差を変えない。同じ入力とシードには同じ結果を返す。
