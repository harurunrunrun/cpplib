---
title: GJK Result 3D (3次元GJK結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/gjk_result3.hpp
---

## API

- `GJKSupportPoint3`: ミンコフスキー差上の点 `point` と、その元になった各形状上の点 `point_on_first`, `point_on_second` を保持する。
- `GJKResult3::intersects`: 2 つの形状が接触または交差するとき `true`。
- `distance`: 交差しないときの距離。交差するときは $0$。
- `point_on_first`, `point_on_second`: 距離を実現する各形状上の点。
- `normal`: 交差しないときの、第 1 の形状から第 2 の形状へ向かう単位ベクトル。
- `simplex`: 終了時のミンコフスキー差上の単体（`std::vector<GJKSupportPoint3>`）。EPA の初期値に利用できる。
- `iterations`, `converged`: 反復回数と、収束したかどうか。

## API別の時間計算量・空間計算量

- 構築とスカラーのメンバの参照: $O(1)$。`simplex` のコピーは要素数を $S \le 4$ として時間・領域 $O(S)$。

## 注意点

- 交差するときの `normal` の値は規定しない。規定される場合は有限な単位ベクトルである。
- 最近点は一意でない場合がある。
- `distance`、両方の最近点、`simplex` は、内部の正規化された座標系の値ではなく、入力座標系に復元した値である。
