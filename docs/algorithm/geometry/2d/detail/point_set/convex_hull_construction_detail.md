---
title: Convex Hull Detail (凸包の内部処理)
documentation_of: ../../../../../../src/algorithm/geometry/2d/detail/point_set/convex_hull_construction_detail.hpp
---

## API

すべて名前空間 `convex_hull_detail` に属する。

- `sorted_unique_points(points)`: 点を辞書順に整列し、座標が完全に一致する重複を除く。
- `build_chain_from_sorted(points, upper, keep_collinear)`: 整列済みの点列から上側または下側のチェーンを作る。
- `build_convex_hull(points, keep_collinear)`: Andrew のモノトーンチェーン法で凸包を作る。

## 引数

`upper` が真なら上側チェーンを、偽なら下側チェーンを作る。`keep_collinear` が真なら辺上の中間点を残す。

## 戻り値

- `sorted_unique_points`: 辞書順に整列済みの点列。
- `build_chain_from_sorted`: 左端から右端へ向かうチェーン。
- `build_convex_hull`: 辞書順最小の点から始まる反時計回りの凸包頂点列。点が 1 個以下ならそのまま返す。全点が共線のとき、`keep_collinear` が真なら整列済みの全点を、偽なら両端の 2 点を返す。

## API別の時間計算量・空間計算量

- `sorted_unique_points`, `build_convex_hull`: 点数を $N$ として時間 $O(N \log N)$、領域 $O(N)$。
- `build_chain_from_sorted`: 時間 $O(N)$、領域 $O(N)$。

## 注意点

凸包 API 間で実装を共有するための内部関数である。
向き判定には `advanced_geometry_detail::cross_sign` を使う。これは 2 辺を単位ベクトルに正規化した外積（角度の正弦）を `GEOMETRY_EPS` と比較するので、座標を正の定数倍しても結果はほぼ変わらない。ただし長さが `GEOMETRY_EPS` 以下の辺は向き 0 として扱う。
