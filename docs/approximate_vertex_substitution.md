---
title: Vertex Substitution k-Median (頂点置換法によるk-メディアン)
documentation_of: ../src/approximate/facility/vertex_substitution.hpp
---

# Vertex Substitution法

## `vertex_substitution_k_median(distance, initial_facilities, maximum_iterations)`

Teitz–Bart法と同じ1施設交換を行う別名API。返り値、tie-break、入力条件は `teitz_bart_k_median` と同じ。

- 時間計算量: $O(FC+Ik(F-k)(C+k\log k))$
- 追加空間計算量: $O(FC+C+F)$
- 近似: 一般の距離行列に対する近似比は保証しない。

## 注意点

施設・需要点の添字、距離行列の形状、距離の有限性と非負性は各APIの条件に従う。明記した場合を除き、局所探索が大域最適解や一定の近似比を返す保証はない。
