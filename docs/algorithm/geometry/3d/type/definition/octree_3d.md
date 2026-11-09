---
title: 3D Octree (三次元八分木)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/octree_3d.hpp
---

`Octree3D` は、指定した有限な `AABB3` を各軸の中点で 8 分割していく、点を動的に追加できる八分木である。葉の点数が閾値を超えたときだけ子ノードを作る。

## API

- `Octree3D(bounds, leaf_capacity = 8, max_depth = 32)`: 根の境界が `bounds` の空の木を構築する（`explicit`）。
- `insert(point)`: 点を挿入し、0 始まりの不変なインデックス（挿入順）を返す。
- `size()`, `empty()`: 点の数と、空かどうかを返す。
- `bounds()`: 根の境界を返す。
- `point(index)`: インデックス `index` の点を返す。範囲外なら `std::out_of_range` を送出する。
- `range_query(range)`: 閉じた `AABB3` である `range` に含まれる点のインデックスを昇順で返す。
- `nearest(query)`: 最近点を `std::optional<std::pair<std::size_t, long double>>`（インデックスと距離の組）で返す。空なら `std::nullopt` を返し、距離が等しい点が複数あればインデックスが最小のものを返す。`query` は根の境界の外にあってもよい。

## API別の時間計算量・空間計算量

点数を $N$、設定した最大深さを $D$、返す点の数を $K$ とする。

- コンストラクタ、`size`、`empty`、`bounds`、`point`: $O(1)$ 時間。
- `insert`: 一様分布では平均 $O(D)$、最悪 $O(D + C)$ 時間。$C$ はそれ以上分割できない葉に入っている点の数である。
- `range_query`: 空間探索は平均 $O(D + K)$、最悪 $O(N)$。返すインデックスを昇順にソートするため、全体では平均 $O(D + K \log (K + 1))$、最悪 $O(N + K \log (K + 1))$ 時間、返り値に $O(K)$ 領域。
- `nearest`: 平均 $O(D)$、最悪 $O(N)$ 時間（ノード数に比例）。探索用のヒープは最悪でノード数に比例する領域を使う。
- 木全体のノード数は最悪 $O(ND)$ になりうるが、通常は $O(N)$ である。

## 注意点

- `bounds` と挿入する点は有限でなければならず、挿入する点は閉じた根の境界内になければならない。
- `leaf_capacity` は正、`max_depth` は $64$ 以下でなければならない。`bounds` が不正な場合も含め、違反時は `std::invalid_argument` を送出する。`range_query` に不正な `range` を渡した場合や、非有限な `query` を渡した場合も `std::invalid_argument` を送出する。
- 同じ点を複数回挿入できる。最大深さに達するか箱がそれ以上分割できなくなると分割を止めるため、無限再帰にはならない。
- 分割面上の点は（中点以上の側の）一意な子に格納するが、`range_query` の範囲判定は閉集合として行う。
