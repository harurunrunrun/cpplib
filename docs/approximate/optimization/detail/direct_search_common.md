---
title: Direct Search Common Helpers (直接探索共通補助)
documentation_of: ../../../../src/approximate/optimization/detail/direct_search_common.hpp
---

Powell 法などの方向探索で共有する内部 API である。以下、次元を $D$ とする。

## API

### `validate_direct_line_parameters(initial_step, reduction, line_tolerance)`

ステップ幅、縮小率、停止幅を検査する。時間計算量・追加空間計算量は $O(1)$ である。

### `displaced_point(point, direction, distance, lower, upper)`

指定した方向へ移動した点を作り、箱制約へ射影する。時間計算量・追加空間計算量は $O(D)$ である。

### `DirectionalSearchResult<Real>`

探索後の点・目的値・移動の有無・評価上限への到達の有無を保持する。空間計算量は $O(D)$ である。

### `directional_pattern_search(point, value, direction, objective, evaluations, max_evaluations, initial_step, reduction, line_tolerance, lower, upper)`

正負の両方向を試し、改善が続く間は同じ幅で進み、改善しなければ幅を縮小する。幅の段階数を $L$、評価回数を $E\le 2L$、目的関数の時間・空間を $T_F, S_F$ として、時間計算量は $O(E(T_F+D))$、追加空間計算量は $O(D+S_F)$ である。

### `point_displacement(point, origin)`

2 点の差のベクトルを返す。時間計算量・追加空間計算量は $O(D)$ である。

## 注意点

- `approximate::optimization::detail` の内部 API であり、次元が一致していることは呼び出し側の前提である
- 不正なパラメータには `std::invalid_argument`、非有限値や算術オーバーフローには `std::overflow_error`、ステップ幅をそれ以上縮小できない場合は `std::runtime_error` を送出する
