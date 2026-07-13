---
title: Persistent Functional Wavelet Matrix (完全永続多機能ウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/persistent_functional_wavelet_matrix.hpp
---

`PersistentWeightedWaveletMatrix` の重みを値自身にしたもの。
更新・クエリ・メモリ計算量は `PersistentWeightedWaveletMatrix` と同じ。
`fork` は $O(1)$。

# 時間計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$D=\mathtt{BIT\_WIDTH}$、$M=\lceil N/B\rceil$、区間が触れるblock数を $C$、
$M_{max}=\lceil\mathtt{MAX\_SIZE}/B\rceil$、
$Q(L)=B+C(\log B+\log(M+1))$、$S(L)=B+C\log(M+1)$ とする。

- default/vector/array constructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}(B+\log(M_{max}+1))+N\log B)$
- `size`, `versions`, `latest_version`, `fork`: $O(1)$
- `access`, `weight`: $O(\log(M+1))$
- `set`, inherited `set_value`, `set_weight`: $O(B\log B+\log(M+1))$
- `rank`, `range_freq`, `range_sum`: $O(Q(L))$、`sum`: $O(S(L))$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: $O(DQ(L))$
- `sum_k_smallest`, `sum_k_largest`: $O(DQ(L)+L)$

## 注意点

```cpp
int set(int version, int k, T value)
```

指定versionの値と重みを同時に更新した新versionを返す。他のqueryと `fork` は
`PersistentWeightedWaveletMatrix` から継承する。`set_value/set_weight` を単独使用すると
「重み=値」は保たれない。version・点・半開区間・bit幅・容量・例外契約は基底型と同じ。

# Constructor signature

```cpp
PersistentFunctionalWaveletMatrix()
PersistentFunctionalWaveletMatrix(const vector<T>& values)
PersistentFunctionalWaveletMatrix(const array<T, N>& values)
```
