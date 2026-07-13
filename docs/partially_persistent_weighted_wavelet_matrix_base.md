---
title: Partially Persistent Weighted Wavelet Matrix Base (部分永続重み付きウェーブレット行列基底)
documentation_of: ../src/structure/wavelet_matrix/detail/partially_persistent_weighted_wavelet_matrix_base.hpp
---

部分永続Wavelet Matrixラッパーが共有する値・重みqueryの基底実装。

## API

- constructor：空列、または同じ長さの値列と重み列からversion 0を作る。
- `size()` / `versions()` / `latest_version()`：列長・version数・最新versionを返す。
- `access(version,k)` / `weight(version,k)`：指定versionの値・重みを返す。
- `rank(version,value,r)` / `rank(version,value,l,r)`：範囲内の `value` の出現数を返す。
- `kth_smallest` / `kth_largest`：`[l,r)` の0-indexed順序統計量を返す。
- `range_freq(version,l,r,upper)`：`upper` 未満の個数を返す。
- `range_freq(version,l,r,lower,upper)`：値域 `[lower,upper)` の個数を返す。
- `sum(version,l,r)`：重みの総和を返す。
- `range_sum`：指定値域に属する要素の重みの総和を返す。
- `sum_k_smallest` / `sum_k_largest`：値が小さい／大きい先頭 `k` 要素の重み和を返す。
- `prev_value` / `next_value`：`upper` 未満の最大値／`lower` 以上の最小値を返し、存在しなければ `nullopt`。

派生型はprotected `set_latest(k,value,weight)` を使い、最新版から1点更新した新versionを作る。

## 時間計算量

$B=\mathtt{BLOCK\_SIZE}$、長さ $L$ の区間が触れるblock数を $C$、version数を $V$ とする。

- constructor：固定領域初期化とblock構築を合わせて $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}B+N\log B)$
- `size`, `versions`, `latest_version`：$O(1)$
- `access`, `weight`：最新版 $O(1)$、過去version $O(\log(V+1))$
- `rank`, `range_freq`, `range_sum`：最新版 $O(B+C\log B)$、過去version $O(B+C(\log B+\log(V+1)))$
- `sum`：最新版 $O(B+C)$、過去version $O(B+C\log(V+1))$
- 順序統計と前後値：上記count時間の $\mathtt{BIT\_WIDTH}$ 倍
- `sum_k_smallest`, `sum_k_largest`：順序統計時間に $O(L)$ を加えた時間
- `set_latest`：$O(B\log B)$

## 注意点

- 公開部分永続Wavelet Matrix群の内部基底であり、通常は直接使用しない。
- 更新は最新版だけから行う。queryは任意の有効versionを参照できる。
- `T` は整数型。符号付き型では `BIT_WIDTH` を型の全bit幅にする。値列と重み列は同じ長さで最大 `MAX_SIZE`。
- 区間は `0 <= l <= r <= size()`、順序 `k` は `[0,r-l)`、値域は `[lower,upper)`。version・点・区間・順序・bit幅・容量の違反では `runtime_error`。
