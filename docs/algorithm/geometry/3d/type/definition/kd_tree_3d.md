---
title: 3D kd-tree (三次元kd木)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/kd_tree_3d.hpp
---

`KdTree3D` は、有限な三次元点の集合を広がり（最大値と最小値の差）が最大の軸の中央値で再帰的に分割し、各部分木の `AABB3` を使って厳密な最近傍探索を行う静的な kd-tree である。

## API

点数を $N$ とする。インデックスは構築時の `points` における位置（0-indexed）である。

- `KdTree3D()`: 空の木を構築する。
- `KdTree3D(points)`: `points`（`std::vector<Point3>`）をコピーして木を構築する（`explicit`）。
- `build(points)`: 現在の内容を破棄し、点集合から再構築する。
- `size()`, `empty()`: 格納している点の数と、空かどうかを返す。
- `point(index)`: インデックス `index` の点を返す。範囲外なら `std::out_of_range` を送出する。
- `nearest(query)`: 最近傍点を `std::optional<std::pair<std::size_t, long double>>`（インデックスと距離の組）で返す。木が空なら `std::nullopt` を返す。距離が等しい点が複数あればインデックスが最小のものを選ぶ。
- `k_nearest(query, count)`: 近い順に高々 `count` 個の `{index, distance}` を `std::vector` で返す。距離が等しい場合はインデックスの昇順に並べる。
- `radius_query(query, radius)`: 中心 `query`、半径 `radius` の閉球に含まれるすべての点を、距離・インデックスの順に並べて返す。

## API別の時間計算量・空間計算量

`k_nearest` では $K$ を `count` と $N$ の小さい方、`radius_query` では $K$ を返す点の数とする。

- `build` とコンストラクタ: 平均・最悪 $O(N \log N)$ 時間、$O(N)$ 領域。
- `nearest`: 平均 $O(\log N)$、最悪 $O(N)$ 時間。再帰のスタックは $O(\log N)$ （退化した入力でも中央値で分割するため、木の高さは $O(\log N)$）。
- `k_nearest`: 平均 $O(\log N + K \log K)$、最悪 $O(N \log K)$ 時間。返り値とヒープに $O(K)$ 領域。
- `radius_query`: 空間探索は平均 $O(\log N + K)$、最悪 $O(N)$。返す列を距離・インデックスの順にソートするため、全体では平均 $O(\log N + K \log (K + 1))$、最悪 $O(N + K \log (K + 1))$ 時間、返り値に $O(K)$ 領域。
- `size`, `empty`, `point`: 時間・追加領域 $O(1)$。

## 注意点

- 入力点とクエリ点は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- `radius` は有限かつ非負でなければならず、違反時は `std::invalid_argument` を送出する。
- `k_nearest` で `count` が $0$ または木が空の場合は空の列を返す。
- 動的な更新は行わない。点集合を変更した場合は `build` で再構築する。
- 距離と `AABB3` による下界の計算には `std::hypot` を使い、距離の 2 乗によるオーバーフローを避ける。
