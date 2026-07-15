---
title: GJK Result 3D (3次元GJK結果)
documentation_of: ../src/algorithm/geometry/3d/gjk_result3.hpp
---

## API

- `GJKSupportPoint3`: Minkowski 差上の点 `point` と、その元になった `point_on_first`, `point_on_second` を保持する。
- `GJKResult3::intersects`: 2 形状が接触または交差するとき真。
- `distance`: 非交差時の距離。交差時は `0`。
- `point_on_first`, `point_on_second`: 距離を実現する各形状上の点。
- `normal`: 非交差時に第1形状から第2形状を向く単位ベクトル。
- `simplex`: 終了時の Minkowski 単体。EPA の初期値に利用できる。
- `iterations`, `converged`: 反復回数と収束フラグ。

## API別の時間計算量・空間計算量

- 各構造体の構築とスカラー場参照は $O(1)$。`simplex` のコピーは要素数を $S\le4$ として $O(S)$ 時間・領域。

## 注意点

`normal` は交差時には規定しない。最近点は一意でない場合がある。

`distance`、両最近点、`simplex` は正規化coreの値ではなく入力座標系へ復元済み。`normal` が規定される場合は有限な単位ベクトルである。
