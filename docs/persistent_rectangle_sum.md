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
