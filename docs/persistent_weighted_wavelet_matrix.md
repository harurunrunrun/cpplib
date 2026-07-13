---
title: Persistent Weighted Wavelet Matrix (完全永続重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/persistent_weighted_wavelet_matrix.hpp
---

完全永続の重み付きwavelet matrix。任意のバージョンから値と重みを更新できる。

# 更新

```cpp
wm.set(version, k, value, weight)
wm.set_value(version, k, value)
wm.set_weight(version, k, weight)
wm.fork(version)
```

# 和

```cpp
wm.sum(version, l, r)
wm.range_sum(version, l, r, upper)
wm.range_sum(version, l, r, lower, upper)
wm.sum_k_smallest(version, l, r, k)
wm.sum_k_largest(version, l, r, k)
```

# 時間計算量

`PersistentWaveletMatrix` と同じ永続ブロック参照を使う。
`set` は $O(B\log B + \log M)$、`fork` は $O(1)$。
`sum`, `range_sum` は $O(B + C(\log B + \log M))$。
`sum_k_smallest` はこれらに加え、境界値と等しい要素の処理に $O(r-l)$ かかる。

$B = BLOCK\_SIZE$、$M = \lceil N / B \rceil$、
$C$ は区間が触れるブロック数とする。

# 計算量（公開操作別）

$D=\mathtt{BIT\_WIDTH}$、$M=\lceil N/B\rceil$、
$M_{max}=\lceil\mathtt{MAX\_SIZE}/B\rceil$、
$Q(L)=B+C(\log B+\log(M+1))$、$S(L)=B+C\log(M+1)$ とする。

- default/vector/array constructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}(B+\log(M_{max}+1))+N\log B)$
- `size`, `versions`, `latest_version`, `fork`: $O(1)$
- `access`, `weight`: $O(\log(M+1))$
- `set`, `set_value`, `set_weight`: $O(B\log B+\log(M+1))$
- `rank`, `range_freq`, `range_sum`: $O(Q(L))$
- `sum`: $O(S(L))$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: $O(DQ(L))$
- `sum_k_smallest`, `sum_k_largest`: $O(DQ(L)+L)$。境界値と等しい要素を元の添字順に拾う走査が $O(L)$

## 注意点

version 0が初期列。`set` は指定versionから1点を置換した新version、`fork` は同内容の
新versionを返す。`access` と全queryは先頭にversionを受け取る。`size/versions/latest_version`
は列長・version数・最新番号を返す。

点・順序統計は0-indexed。列区間は半開区間 `[l,r)`、値域は
`[lower,upper)`。前後値は該当なしなら `nullopt`。
不正なversion・点・区間・順序・値域、bit幅、列長/version/block node容量超過では
`runtime_error`。失敗した更新は使用量を戻す。各APIの計算量は上記表の通り。

`set` は値と重み、`set_value/set_weight` は片方を更新する。`weight` は1点の重み、
`sum/range_sum` は全値域・指定値域、`sum_k_smallest/sum_k_largest` は順序先頭の重みを返す。

# Constructor signature

```cpp
PersistentWeightedWaveletMatrix()
PersistentWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
PersistentWeightedWaveletMatrix(const array<T, N>& values, const array<W, N>& weights)
```
