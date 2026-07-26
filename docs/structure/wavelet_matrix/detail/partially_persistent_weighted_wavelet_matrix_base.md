---
title: Partially Persistent Weighted Wavelet Matrix Base (部分永続重み付きウェーブレット行列基底)
documentation_of: ../../../../src/structure/wavelet_matrix/detail/partially_persistent_weighted_wavelet_matrix_base.hpp
---

部分永続重み付きWavelet Matrixの内部基底。完全永続基底をincludeまたは継承せず、
各bit段のpayload和付き不変B+木とlatest-only更新を独立に実装する。通常は公開ラッパーを使用する。

## API

公開queryは `size/versions/latest_version/access/weight/rank/select/kth_smallest/kth_largest/`
`range_freq/sum/range_sum/sum_k_smallest/sum_k_largest/prev_value/next_value`。
派生型はprotected `set_latest` と `set_weight_latest` を使う。

## 時間計算量

$D=\mathtt{BIT\_WIDTH}$、$H=O(\log(N+1))$。

- 構築: $O(DN)$
- `size/versions/latest_version`: $O(1)$
- `weight/sum`: $O(H)$
- その他の公開query: $O(DH)$
- `set_latest/set_weight_latest`: $O(DH)$

構築メモリは $O(DN)$、更新1回の追加メモリは $O(DH)$。過去versionも同じ時間でqueryできる。

## 注意点

更新元は最新版だけで、既存versionは不変。`W{}`, 加算、減算を重み和に使う。
符号付き `T` は全bit幅を指定する。不正なversion・範囲・順序・bit幅・容量は `runtime_error`。
更新失敗時は各B+木のsnapshotへrollbackする。
