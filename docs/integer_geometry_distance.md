---
title: Exact Integer Point-Set Distances (厳密整数点集合距離)
documentation_of: ../src/algorithm/integer_geometry/distance.hpp
---

## API

- `squared_distance(a, b)`: 二点間距離の二乗を `__uint128_t` で返す。
- `closest_pair_squared(points)`: 最近点対距離の二乗を返す。点が二つ未満なら `std::nullopt`。
- `convex_diameter_squared(polygon)`: 凸多角形の直径の二乗を回転キャリパー法で返す。空なら `std::nullopt`。
- `point_set_diameter_squared(points)`: 点集合の凸包を構築し、直径の二乗を返す。空なら `std::nullopt`。

## API別の時間計算量・空間計算量

- `squared_distance`: 時間・追加領域 $O(1)$。
- `closest_pair_squared`: 点数を $N$ として時間 $O(N\log N)$、領域 $O(N)$。
- `convex_diameter_squared`: 凸包頂点数を $H$ として時間 $O(H)$、追加領域 $O(1)$。
- `point_set_diameter_squared`: 点数を $N$ として時間 $O(N\log N)$、領域 $O(N)$。

## 注意点

平方根を取らないため誤差はない。`squared_distance` は指定された距離が `__uint128_t` で表せなければ `std::overflow_error` を送出する。`closest_pair_squared` は遠い候補対が表現範囲外でも無視して正しい最小値を求め、最小距離そのものが表現できない場合だけ `std::overflow_error` を送出する。

回転キャリパーの面積比較は最大129 bitの絶対値を二語で保持して正確に行う。最初の辺について全頂点を一度走査して対蹠点を初期化し、等しい面積の対蹠点も比較するため、時計回り・反時計回りのどちらでも辺上の共線頂点を取りこぼさない。全点共線なら辞書順の両端を $O(H)$ 時間・$O(1)$ 追加領域で直接比較する。直径の平方そのものが `__uint128_t` で表せない場合は `std::overflow_error` を送出する。`convex_diameter_squared` には凸多角形を巡回順で渡し、重複頂点を含めない。
