---
title: Persistent Weighted Wavelet Matrix (完全永続重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/persistent/persistent_weighted_wavelet_matrix.hpp
---

各要素に重みを持ち、任意の有効versionから点更新・分岐できる完全永続Wavelet Matrix。
各bit段をpayload和付きの永続B+木で管理し、古いversionを変更しない。

## API

```cpp
PersistentWeightedWaveletMatrix<T, W, MAX_SIZE, MAX_VERSION, BIT_WIDTH> wm(values, weights)
int n = wm.size()
int count = wm.versions()
int latest = wm.latest_version()
T value = wm.access(version, k)
W weight = wm.weight(version, k)
int version2 = wm.set(version, k, value, weight)
int version2 = wm.set_value(version, k, value)
int version2 = wm.set_weight(version, k, weight)
int version2 = wm.fork(version)
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

version 0が初期列。更新は新しいversion番号を返す。
`select` は指定値の0-indexed `occurrence` 番目の位置を返し、存在しなければ `size()` を返す。
区間と値域は半開区間である。同値をまたぐ `sum_k_*` は現在の列での安定順に重みを選ぶ。

## 時間計算量

$D=\mathtt{BIT\_WIDTH}$、$H=O(\log(N+1))$ とする。

- vector/array constructor: $O(DN)$、default constructor: $O(D)$
- `size`, `versions`, `latest_version`: $O(1)$
- `weight`, `sum`: $O(H)$
- `access`, `rank`, `select`, `kth_smallest`, `kth_largest`: $O(DH)$
- `range_freq`, `range_sum`, `sum_k_smallest`, `sum_k_largest`: $O(DH)$
- `prev_value`, `next_value`: $O(DH)$
- `set`, `set_value`, `set_weight`: $O(DH)$
- `fork`: $O(D)$

初期追加メモリは $O(DN)$、更新1回の追加メモリは $O(DH)$、`fork` は $O(D)$。
平方分割には依存しない。

## 注意点

`T` はbool以外の整数型。符号付き `T` では `BIT_WIDTH` は型の全bit幅でなければならない。
`W{}` を加法単位元として `W + W` と `W - W` が定義されている必要がある。
`MAX_SIZE` は列長、`MAX_VERSION` は初期version 0の後に作れるversion数の上限。

不正なversion・添字・区間・順序・値域・bit幅・容量では `runtime_error`。
B+木の更新途中で容量確保や重み演算が失敗した場合も、全bit段のnode使用量とversion数を更新前へ戻す。
