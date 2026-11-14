---
title: Offline Indexed Rectangle Affine Sum (点 ID 付き長方形アフィン作用と和) [dynamic_point_set_rectangle_affine_rectangle_sum]
documentation_of: ../../../src/structure/spatial/offline_indexed_rectangle_affine_sum.hpp
---

全点の座標をコンストラクタ `OfflineIndexedRectangleAffineSum<T>(coordinates)` で先に登録し、
点の有効化・無効化、長方形内の有効な点へのアフィン作用、長方形内の和を処理する。
`coordinates` は `std::vector<std::pair<int64_t, int64_t>>` である。
点 ID は登録順の添字（0-indexed）で、各点は最初は無効である。同じ座標の点もそれぞれ独立に管理する。

## API

- `point_set(id, value)`: 点 `id` を有効化するか、既に有効なら現在値を `value` に置き換える。
- `point_erase(id)`: 点 `id` を無効化する。以前有効だった場合に `true` を返す。再び `point_set` すると有効化される。
- `rectangle_affine(l, d, r, u, a, b)`: 半開長方形 `[l, r) × [d, u)` 内の現在有効な点の値 `value` を `a * value + b` に置き換える。
- `rectangle_sum(l, d, r, u)`: 半開長方形 `[l, r) × [d, u)` 内の現在有効な点の値の総和を返す。遅延作用を伝播するため非 `const` である。
- `size()`: 登録した点数を返す。

## 時間計算量

登録点数を $N$ とする。構築は `nth_element` の平均線形性のもとで $O(N\log N)$、
点の更新は $O(\log N)$、長方形への作用・和の取得は $O(\sqrt N)$ である。空間計算量は $O(N)$ である。

## 注意点

- 無効だった期間に行った作用は、その後に有効化した点には適用されない。
- 座標は負の値を含む `int64_t` である。長方形は `l <= r` かつ `d <= u` を要求し、逆順なら `invalid_argument` を送出する。
- `id` が `[0, size())` の範囲外なら `out_of_range` を送出する。
- `T` は整数からの構築、比較、加算・乗算を提供し、中間結果を表現できる必要がある。
