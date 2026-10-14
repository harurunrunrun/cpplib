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

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE`, `MAX_VERSION` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = PersistentFunctionalWaveletMatrix<T>;
using Fixed = PersistentFunctionalWaveletMatrix<T, N, Versions>;
```

可変長版は必要なノード・版・履歴を明示的に追加し、保存要素数を $K$ として追加領域は $O(K)$。
再確保の費用は追加操作全体で償却され、上記各操作の計算量を維持する。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
