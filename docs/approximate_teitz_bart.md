---
title: Teitz–Bart k-Median (Teitz–Bart法によるk-メディアン)
documentation_of: ../src/approximate/facility/teitz_bart.hpp
---

# Teitz–Bart法

現在開いている施設1個と閉じている施設1個を交換し、距離和が最も減る交換を反復するVertex Substitution法。

## `teitz_bart_k_median(distance, initial_facilities, maximum_iterations)`

初期施設数を保ったまま1交換局所最適化を行い、`KMedianResult` を返す。同じ改善量なら交換後の施設列が辞書順最小のものを選ぶ。`maximum_iterations == 0` では初期解の評価だけを行う。

- 1反復の時間計算量: $O(k(F-k)(C+k\log k)+kC)$
- 全体の時間計算量: $O(FC+Ik(F-k)(C+k\log k))$
- 追加空間計算量: $O(FC+C+F)$
- 近似: 一般の距離行列では大域最適性を保証しない。終了時は調べた全1交換で真に改善しないか、反復上限に達している。

初期施設は空でなく、範囲内で相異なる必要がある。
