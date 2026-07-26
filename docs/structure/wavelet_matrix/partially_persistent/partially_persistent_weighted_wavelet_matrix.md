---
title: Partially Persistent Weighted Wavelet Matrix (部分永続重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_weighted_wavelet_matrix.hpp
---

各要素に重みを持つ部分永続Wavelet Matrix。更新元は最新版だけで、すべての過去versionを参照できる。
完全永続型をラップせず、latest-onlyの更新経路を各bit段のpayload和付き不変B+木で独立に実装する。

## API

```cpp
PartiallyPersistentWeightedWaveletMatrix<
    T, W, MAX_SIZE, MAX_VERSION, BIT_WIDTH
> wm(values, weights)
int n = wm.size()
int count = wm.versions()
int latest = wm.latest_version()
T value = wm.access(version, k)
W weight = wm.weight(version, k)
int version2 = wm.set(k, value, weight)
int version2 = wm.set_value(k, value)
int version2 = wm.set_weight(k, weight)
int count = wm.rank(version, value, r)
int count = wm.rank(version, value, l, r)
int position = wm.select(version, value, occurrence)
T value = wm.kth_smallest(version, l, r, k)
T value = wm.kth_largest(version, l, r, k)
int count = wm.range_freq(version, l, r, upper)
int count = wm.range_freq(version, l, r, lower, upper)
W total = wm.sum(version, l, r)
W total = wm.range_sum(version, l, r, upper)
W total = wm.range_sum(version, l, r, lower, upper)
W total = wm.sum_k_smallest(version, l, r, k)
W total = wm.sum_k_largest(version, l, r, k)
optional<T> value = wm.prev_value(version, l, r, upper)
optional<T> value = wm.next_value(version, l, r, lower)
```

version 0が初期列。`set`, `set_value`, `set_weight` は最新版から新versionを作る。
queryは任意の有効versionを読む。`select` が見つからない場合は `size()`。
区間と値域は半開区間で、同値をまたぐ `sum_k_*` は現在の列での安定順に重みを選ぶ。

## 時間計算量

$D=\mathtt{BIT\_WIDTH}$、$H=O(\log(N+1))$ とする。最新版と過去versionで同じ計算量である。

- vector/array constructor: $O(DN)$、default constructor: $O(D)$
- `size`, `versions`, `latest_version`: $O(1)$
- `weight`, `sum`: $O(H)$
- `access`, `rank`, `select`, 順序統計、値域頻度・値域和、前後値: $O(DH)$
- `sum_k_smallest`, `sum_k_largest`: $O(DH)$
- `set`, `set_value`, `set_weight`: $O(DH)$

初期追加メモリは $O(DN)$、更新1回の追加メモリは $O(DH)$。
平方分割やversion履歴の線形走査には依存しない。

## 注意点

`T` はbool以外の整数型。符号付き `T` では `BIT_WIDTH` は型の全bit幅。
`W{}` を加法単位元として `W + W` と `W - W` が必要。
`MAX_SIZE` は列長、`MAX_VERSION` は初期version後に作れるversion数の上限。
不正なversion・添字・区間・順序・値域・bit幅・容量では `runtime_error`。
失敗した更新は全bit段のnode使用量とversion数を更新前へ戻す。
