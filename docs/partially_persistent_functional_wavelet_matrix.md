---
title: Partially Persistent Functional Wavelet Matrix (部分永続多機能ウェーブレット行列)
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

# API契約

```cpp
int set(int k, T value)
```

最新版の値と重みを同時更新した新versionを返す。queryは任意versionを参照できる。
他の公開APIは `PartiallyPersistentWeightedWaveletMatrix` から継承する。
`set_value/set_weight` の単独使用では「重み=値」は保たれない。
version・点・半開区間・bit幅・容量・例外契約は基底型と同じ。

# Constructor signature

```cpp
PartiallyPersistentFunctionalWaveletMatrix()
PartiallyPersistentFunctionalWaveletMatrix(const vector<T>& values)
PartiallyPersistentFunctionalWaveletMatrix(const array<T, N>& values)
```
