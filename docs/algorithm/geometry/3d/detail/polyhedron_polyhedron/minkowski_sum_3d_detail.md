---
title: Minkowski Sum 3D Internal Detail (三次元Minkowski和内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/polyhedron_polyhedron/minkowski_sum_3d_detail.hpp
---

3 つの公開個別ヘッダが共有する、候補の生成と直接構築の本体である。

## API

- `minkowski_sum_3d_detail::candidate_points(first, second)`: 全頂点対の和を生成し、辞書順にソートして完全一致する重複を除いて返す内部関数。どちらかが空なら `std::invalid_argument`、候補数が `std::vector` の上限を超えるなら `std::length_error` を送出する。
- `minkowski_sum_3d_detail::direct_sum(first, second, margin)`: ファセットと法線ファンを統合して Minkowski 和を直接構築する本体。`margin` は $0 < \mathrm{margin} < 0.01$ の有限値でなければならない。
- その他の名前空間内の要素: ファセットの統合、支持点の探索、出力の位相の検証のための内部型・関数。

## API別の時間計算量・空間計算量

頂点数を $V_1, V_2$、面数を $F_1, F_2$、三角形数を $T_1, T_2$、辺数を $E_1, E_2$、出力サイズを $K$ とする。

- `candidate_points`: $P=V_1V_2$ に対して、ソートを含めて時間 $O(P\log P)$、領域 $O(P)$。
- `direct_sum`: 期待時間 $O(T_1+T_2+F_1V_2+F_2V_1+E_1E_2+K)$、追加領域 $O(V_1+V_2+T_1+T_2+K)$。

## 注意点

内部実装用のヘッダであり、名前・引数・返り値の互換性は公開 API として保証しない。
利用側は 3 つの公開個別ヘッダ、または互換用の集約ヘッダを include すること。
