---
title: Teitz–Bart k-Median (Teitz–Bart法によるk-メディアン)
documentation_of: ../../../src/approximate/facility/teitz_bart.hpp
---

開いている施設 1 個と閉じている施設 1 個を交換し、距離の和が最も減る交換を繰り返す Vertex Substitution 法である。以下、候補施設数を $F$、需要点数を $C$、初期施設数を $k$、採用した交換の回数を $I$ とする。

## `teitz_bart_k_median(distance, initial_facilities, maximum_iterations = 100)`

初期施設数を保ったまま 1 交換の局所最適化を行い、`KMedianResult` を返す。改善量が同じなら、交換後の施設列が辞書順最小のものを選ぶ。`maximum_iterations == 0` では初期解の評価だけを行う。終了時は、調べた全ての 1 交換で真に改善しないか、反復回数の上限に達している。

- 1 反復の時間計算量: $O(k(F-k)(C+k\log k)+kC)$
- 全体の時間計算量: $O(FC+Ik(F-k)(C+k\log k))$
- 追加空間計算量: $O(FC+C+F)$

## 注意点

- 初期施設は空でなく、範囲内で相異なっていなければならない。
- 距離行列は `distance[facility][client]` 形式の長方形で、全要素が有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 一般の距離行列では大域最適性を保証しない。
