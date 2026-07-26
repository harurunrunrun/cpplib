---
title: Persistent Functional Wavelet Matrix (完全永続多機能ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_functional_wavelet_matrix.hpp
---

値自身を重みとする `PersistentWeightedWaveletMatrix`。任意versionから `set(version,k,value)`
で値と重みを同時更新し、頻度・順序統計・値域和を過去の全versionで利用できる。

## API

コンストラクタは空列、`vector<T>`、`array<T,N>`。`set(version,k,value)` 以外は
基底の `size/versions/latest_version/access/weight/fork/rank/select/kth_*/range_freq/`
`sum/range_sum/sum_k_*/prev_value/next_value` を利用できる。
継承した `set_value/set_weight` を単独で使うと「重み=値」は保たれない。

## 時間計算量

$D=\mathtt{BIT\_WIDTH}$、$H=O(\log(N+1))$。

- 構築: $O(DN)$
- `size/versions/latest_version`: $O(1)$、`fork`: $O(D)$
- `weight/sum`: $O(H)$
- `set` とその他のquery: $O(DH)$

更新1回の追加メモリは $O(DH)$。

## 注意点

`Sum` は `T` から変換でき、`Sum{}`、加算、減算を使える型とする。
version・添字・半開区間・bit幅・容量・例外時rollbackの契約は基底型と同じ。
