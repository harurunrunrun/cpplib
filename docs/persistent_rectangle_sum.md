---
title: Persistent Rectangle Sum
documentation_of: ../src/structure/wavelet_matrix/persistent_rectangle_sum.hpp
---

`PersistentWaveletMatrix2DWeighted` の矩形和用の別名。
更新・クエリ・メモリ計算量は `PersistentWaveletMatrix2DWeighted` と同じ。

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$M=\lceil N/B\rceil$、
$M_{max}=\lceil\mathtt{MAX\_SIZE}/B\rceil$ とする。
x範囲に入る $L$ 点が触れるblock数を $C$ とし、
$Q(L)=B+C(\log B+\log(M+1))$ とおく。

- 3種類のconstructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}(B+\log(M_{max}+1))+N(\log N+\log B))$
- `size`, `versions`, `latest_version`, `x`, `fork`: $O(1)$
- `y`, `weight`: $O(\log(M+1))$
- `set`, `set_y`, `set_weight`: $O(B\log B+\log(M+1))$
- `rectangle_count`, `range_freq`, `rectangle_sum`, `range_sum`: $O(\log N+Q(L))$

# API契約

基底型の `size/versions/latest_version/x/y/weight/set/set_y/set_weight/fork/`
`rectangle_count/range_freq/rectangle_sum/range_sum` を利用できる。更新は指定versionから
新versionを返し、矩形は `[xl,xr) x [yl,yr)`。不正version・点・容量・bit幅・矩形では
`runtime_error`。各APIの計算量は上記表の通り。
