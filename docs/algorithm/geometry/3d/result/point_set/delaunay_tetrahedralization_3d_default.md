---
title: 3D Delaunay Tetrahedralization with Default Seed (固定シード三次元Delaunay四面体分割)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point_set/delaunay_tetrahedralization_3d_default.hpp
---

固定シードで挿入順を決める乱択 Bowyer–Watson 法により、三次元 Delaunay 四面体分割を構築する。

## API

- `delaunay_tetrahedralization_3d(points)`: 完全一致する重複点を除いたうえで、固定シードの Bowyer–Watson 法で `DelaunayTetrahedralization3` を返す。同じ入力からは常に同じ四面体列が得られる。

## API別の時間計算量・空間計算量

点数を $N$、出力と増分構築中に生成される四面体および衝突（conflict）の総数を $K$、検査した衝突対の数を $C$ とすると、時間 $O(N \log N + K + C)$、追加空間 $O(N + K)$。最悪の場合は $K = O(N^2)$、$C = O(NK)$ になる。

このほか、構築結果の検証として凸包の計算と体積の厳密比較を行う。検証に失敗した場合は、外側の四面体（super tetrahedron）を拡大しシードを変えて最大 $12$ 回まで再構築する。

## 注意点

- 座標は有限でなければならず、違反した場合は `std::invalid_argument` を送出する。
- 共線・共面の入力では四面体を生成しない（`tetrahedra` は空）。
- 有限な外側の四面体で覆えない入力には `std::overflow_error` を送出する。
- 乱択版は `delaunay_tetrahedralization_3d_randomized` を参照。
