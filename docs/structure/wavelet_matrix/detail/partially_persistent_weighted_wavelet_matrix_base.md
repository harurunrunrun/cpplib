---
title: Partially Persistent Weighted Wavelet Matrix Base (部分永続重み付きウェーブレット行列基底)
documentation_of: ../../../../src/structure/wavelet_matrix/detail/partially_persistent_weighted_wavelet_matrix_base.hpp
---

部分永続の重み付き Wavelet Matrix の内部基底である。完全永続版の基底を include も継承もせず、
各ビット段のペイロード和付き不変 B+ 木（`ImmutableBTreeBitSequence`）と、最新版だけを更新元とする更新を独立に実装する。通常は公開ラッパーを使用する。

## API

```cpp
wavelet_matrix_detail::PartiallyPersistentWeightedWaveletMatrixBase<T, W, MAX_SIZE = -1, MAX_VERSION = -1, BIT_WIDTH = (T の符号なし版のビット幅)>
```

公開クエリは `size`、`versions`、`latest_version`、`access`、`weight`、`rank`、`select`、`kth_smallest`、`kth_largest`、
`range_freq`、`sum`、`range_sum`、`sum_k_smallest`、`sum_k_largest`、`prev_value`、`next_value` である。`size` などを除き、版の番号を第 1 引数に取る。
派生型は protected の `set_latest` と `set_weight_latest` を使う。

## 時間計算量

$D$ を `BIT_WIDTH`、$N$ を列長、$H=O(\log(N+1))$ を各段の B+ 木の高さとする。

- 構築: $O(DN)$
- `size`、`versions`、`latest_version`: $O(1)$
- `weight`、`sum`: $O(H)$
- その他の公開クエリ: $O(DH)$
- `set_latest`、`set_weight_latest`: $O(DH)$

構築時のメモリは $O(DN)$、更新 1 回あたりの追加メモリは $O(DH)$ である。過去の版も同じ時間計算量でクエリできる。

## 注意点

- 更新元は最新版だけで、既存の版は不変である。
- 重みの和には `W{}`、加算、減算を使う。
- 符号付きの `T` を使う場合は、全ビット幅を `BIT_WIDTH` に指定する。
- 不正な版・範囲・順序・ビット幅・容量では `runtime_error` を送出する。
- 更新に失敗した場合は、各 B+ 木のスナップショットへ巻き戻す。
- `MAX_SIZE` と `MAX_VERSION` は省略でき、デフォルト値 `-1` では `std::vector` を使う可変長版になる。容量を指定した固定配列版（`wavelet_matrix_detail::PartiallyPersistentWeightedWaveletMatrixBase<T, W, N, Versions>`）も同じヘッダで利用できる。可変長版は必要なノード・版・履歴を追加し、保存要素数を $K$ として追加領域は $O(K)$ である。再確保の費用はならして上記の計算量に含まれる。
