---
title: Partially Persistent Functional Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_functional_wavelet_matrix.hpp
---

`PartiallyPersistentWeightedWaveletMatrix` の重みを値自身にしたもの。

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$V$ を現在のversion数、区間長を $L$、触れるblock数を $C$ とし、
$Q_0(L)=B+C\log B$、$Q_V(L)=B+C(\log B+\log(V+1))$、
$S_0(L)=B+C$、$S_V(L)=B+C\log(V+1)$ とおく。

- default/vector/array constructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}B+N\log B)$
- `size`, `versions`, `latest_version`: $O(1)$
- `access`, `weight`: 最新versionは $O(1)$、過去versionは $O(\log(V+1))$
- `set`および継承した`set_value`, `set_weight`: 償却 $O(B\log B)$。履歴vector再確保時の1回の最悪は $O(B\log B+V)$
- `rank`, `range_freq`, `range_sum`: 最新versionは $O(Q_0(L))$、過去versionは $O(Q_V(L))$
- `sum`: 最新versionは $O(S_0(L))$、過去versionは $O(S_V(L))$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: 最新versionは $O(\mathtt{BIT\_WIDTH}\,Q_0(L))$、過去versionは $O(\mathtt{BIT\_WIDTH}\,Q_V(L))$
- `sum_k_smallest`, `sum_k_largest`: 上記の順序統計時間に $O(L)$ を加えた時間
