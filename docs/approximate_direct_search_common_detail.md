---
title: Direct Search Common Helpers (直接探索共通補助)
documentation_of: ../src/approximate/optimization/detail/direct_search_common.hpp
---

## Direct Search Common Helpers (直接探索共通補助)

Powell法などの方向探索で共有する内部API。

## API

### `validate_direct_line_parameters(initial_step, reduction, line_tolerance)`

step、縮小率、停止幅を検査する。時間・追加空間計算量は $O(1)$。

### `displaced_point(point, direction, distance, lower, upper)`

指定方向へ移動した点を作り、箱型制約へ射影する。次元を $D$ として時間・追加空間計算量は $O(D)$。

### `DirectionalSearchResult<Real>`

探索後の点・目的値・移動有無・評価上限到達有無を保持する。空間計算量は $O(D)$。

### `directional_pattern_search(...)`

正負両方向を試し、改善中は同じ幅で進み、改善しなければ幅を縮小する。幅の段階数を $L$、評価回数を $E\le 2L$、目的関数の計算量を $T_F,S_F$ として時間計算量は $O(E(T_F+D))$、追加空間計算量は $O(D+S_F)$。

### `point_displacement(point, origin)`

2点の差vectorを返す。時間・追加空間計算量は $O(D)$。

## 注意点

`approximate::optimization::detail` の内部APIである。次元一致は呼出側の前提。不正parameter、非有限値、算術overflow、stepを縮小できない場合を例外として拒否する。
