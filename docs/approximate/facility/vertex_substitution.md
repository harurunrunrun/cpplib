---
title: Vertex Substitution k-Median (頂点置換法によるk-メディアン)
documentation_of: ../../../src/approximate/facility/vertex_substitution.hpp
---

以下、候補施設数を $F$、需要点数を $C$、初期施設数を $k$、採用した交換の回数を $I$ とする。

## `vertex_substitution_k_median(distance, initial_facilities, maximum_iterations = 100)`

Teitz–Bart 法と同じ 1 施設交換を行う別名の API である。戻り値、同値時の選び方、入力条件は `teitz_bart_k_median` と同じである。

- 時間計算量: $O(FC+Ik(F-k)(C+k\log k))$
- 追加空間計算量: $O(FC+C+F)$

## 注意点

- 初期施設は空でなく、範囲内で相異なっていなければならない。
- 距離行列は `distance[facility][client]` 形式の長方形で、全要素が有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 一般の距離行列に対する近似比は保証しない。
