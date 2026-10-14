---
title: Persistent Weighted Wavelet Matrix Base (完全永続重み付きウェーブレット行列基底)
documentation_of: ../../../../src/structure/wavelet_matrix/detail/persistent_weighted_wavelet_matrix_base.hpp
---

完全永続重み付きWavelet Matrixの内部基底。各bit段に
`ImmutableBTreeBitSequence<W,W>` を持ち、bitのrankと重みの部分和を同じ永続B+木で管理する。
通常は公開ラッパーを使用する。

## API

公開queryは `size/versions/latest_version/access/weight/rank/select/kth_smallest/kth_largest/`
`range_freq/sum/range_sum/sum_k_smallest/sum_k_largest/prev_value/next_value`。
派生型はprotected `set_from_version`, `set_weight_from_version`, `fork_from_version` を使う。

## 時間計算量

$D=\mathtt{BIT\_WIDTH}$、$H=O(\log(N+1))$。

- 構築: $O(DN)$
- `size/versions/latest_version`: $O(1)$
- `weight/sum`: $O(H)$
- その他の公開query: $O(DH)$
- `set_from_version/set_weight_from_version`: $O(DH)$
- `fork_from_version`: $O(D)$

構築メモリは $O(DN)$、更新の追加メモリは $O(DH)$。平方分割は使用しない。

## 注意点

任意の有効versionから分岐し、既存versionは不変。`W{}`, 加算、減算を重み和に使う。
符号付き `T` は全bit幅を指定する。不正なversion・範囲・順序・bit幅・容量は `runtime_error`。
更新失敗時は各B+木のsnapshotへrollbackする。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE`, `MAX_VERSION` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = wavelet_matrix_detail::PersistentWeightedWaveletMatrixBase<T, W>;
using Fixed = wavelet_matrix_detail::PersistentWeightedWaveletMatrixBase<T, W, N, Versions>;
```

可変長版は必要なノード・版・履歴を明示的に追加し、保存要素数を $K$ として追加領域は $O(K)$。
再確保の費用は追加操作全体で償却され、上記各操作の計算量を維持する。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
