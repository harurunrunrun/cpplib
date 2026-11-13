---
title: Best-Bin-First KD-Tree (最良ビン優先KD木)
documentation_of: ../../../src/approximate/nearest_neighbor/best_bin_first_kd_tree.hpp
---

部分木の軸平行な包囲箱とクエリ点との距離が小さい順に探索する静的な KD 木である。調べる点数を制限すると近似最近傍探索、制限しなければ厳密な最近傍探索になる。

## 公開型

- `BestBinFirstKdTree<T, Dimension>::Point`: `std::array<T, Dimension>` の別名
- `KdTreeNeighbor::index`: 最近傍として選んだ登録点の添字
- `KdTreeNeighbor::squared_distance`: クエリとの二乗距離
- `KdTreeNeighbor::checked_points`: 実際に距離を評価した点数

各メンバの参照の時間計算量・追加空間計算量は $O(1)$ である。

## `BestBinFirstKdTree()`

点を持たない木を構築する。時間計算量・追加空間計算量は $O(1)$ である。

## `BestBinFirstKdTree(points)` / `reset(points)`

`std::array<T, Dimension>` の点群から平衡な木を構築する。同じ座標の点もすべて保持する。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: 構築後の木と点群を含めて $O(ND)$

## `size()` / `empty()` / `point(index)`

点数、空かどうか、元の添字に対応する点を返す。`point` は範囲外の添字で `std::out_of_range` を送出する。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `nearest(query, max_checks = numeric_limits<size_t>::max())`

`KdTreeNeighbor { index, squared_distance, checked_points }` を返す。木が空なら `std::nullopt` を返す。`max_checks` は実際に距離を評価する点数の上限で、正でなければならない（0 なら `std::invalid_argument`）。距離が等しい場合は元の添字が小さい点を返す。

- 時間計算量: $C$ を `max_checks` として、最悪 $O(\min(N,C)\log N)$、通常は $O(\log N+C\log N)$ 程度
- 追加空間計算量: 最悪 $O(N)$
- 近似: `max_checks >= N`（デフォルトを含む）なら厳密である。小さい値では近似比の保証はない。包囲箱による下界が現在の最良値を超えた部分木は安全に除く

## 注意点

- 登録点とクエリの座標は有限でなければならず、非有限な座標には `std::invalid_argument` を送出する
- 二乗距離を `long double` で表現できない場合は `std::overflow_error` を送出する
- 厳密探索になる場合を除き、探索予算に対する再現率（recall）や近似比は保証しない
