---
title: Dynamic Rectangle Sum (動的長方形和)
documentation_of: ../src/structure/wavelet_matrix/dynamic_rectangle_sum.hpp
---

`DynamicWaveletMatrix2DWeighted` の矩形和用の別名。
x座標は固定、y座標と重みは更新できる。

# 時間計算量

`DynamicWaveletMatrix2DWeighted` と同じ。

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$ とし、x範囲に入る点数を $L$、
$Q(L)=B+\lceil L/B\rceil\log B$ とする。

- default constructor: $O(1)$
- vector/points constructor: $O(\mathtt{MAX\_SIZE}+N(\log N+\log B))$
- `size`, `x`, `y`, `weight`: $O(1)$
- `set`, `set_y`, `set_weight`: $O(B\log B)$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: $O(\log N+Q(L))$

# API契約

基底型の `size/x/y/weight/set/set_y/set_weight/rectangle_count/range_freq/rectangle_sum/range_sum`
を利用できる。点idは入力順、xは固定、矩形は `[xl,xr) x [yl,yr)`。
vector長・点・容量・bit幅・矩形違反では `runtime_error`。各APIの計算量は上記表の通り。
