---
title: Minkowski Sum 3D Internal Detail (三次元Minkowski和内部実装)
documentation_of: ../../../../src/algorithm/geometry/3d/minkowski_sum_3d_detail.hpp
---

三つの公開leafが共有する候補生成と直接構築本体を保持する。

## API

- `minkowski_sum_3d_detail::candidate_points(first, second)`: 全頂点対の和を生成する内部関数。
- `minkowski_sum_3d_detail::direct_sum(first, second, margin)`: facetと法線fanを統合する内部構築本体。
- その他のnamespace内要素: facet統合、支持点探索、出力位相検証用の内部型・関数。

## API別の時間計算量・空間計算量

`candidate_points` は $P=V_1V_2$ に対して $O(P)$ 時間・領域。
`direct_sum` は期待
$O(T_1+T_2+F_1V_2+F_2V_1+E_1E_2+K)$ 時間、
$O(V_1+V_2+T_1+T_2+K)$ 追加領域。

## 注意点

内部実装用headerであり、名前・引数・返り値の互換性は公開APIではない。
利用側は三つの公開leafまたは互換aggregatorをincludeする。
