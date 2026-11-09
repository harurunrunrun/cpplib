---
title: Randomized 3D Delaunay Tetrahedralization (乱択三次元Delaunay四面体分割)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/delaunay_tetrahedralization_3d_randomized.hpp
---

指定したシードで挿入順をシャッフルする乱択 Bowyer–Watson 法により、三次元 Delaunay 四面体分割を構築する。

## API

- `delaunay_tetrahedralization_3d_randomized(points, seed)`: `seed`（`std::uint64_t`）で挿入順をシャッフルし、`DelaunayTetrahedralization3` を返す。同じ入力と同じシードからは同じ結果が得られる。

## API別の時間計算量・空間計算量

点数を $N$、四面体と衝突（conflict）の総数を $K$ とする。一般位置の入力で、入力と独立な乱択順を用いる場合、期待時間 $O(N \log N + K)$、追加空間 $O(N + K)$。三次元では $K = O(N^2)$ になり得る。

このほか、構築結果の検証として凸包の計算と体積の厳密比較を行い、失敗した場合は外側の四面体を拡大しシードを変えて最大 $12$ 回まで再構築する。

## 注意点

- 期待計算量を保証したい場合は、入力から独立なシードを用いること。
- 座標は有限でなければならず、違反した場合は `std::invalid_argument` を送出する。重複点の判定は座標の完全一致で行う。
- 共線・共面の入力では四面体を生成しない。有限な外側の四面体で覆えない入力には `std::overflow_error` を送出する。
