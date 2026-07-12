---
title: Partially Persistent Weighted Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_weighted_wavelet_matrix.hpp
---

各要素に重みを持つ、点更新可能な部分永続 Wavelet Matrix。
更新対象は最新バージョンに限る。過去のバージョンはクエリで参照できる。

# コンストラクタ

```cpp
PartiallyPersistentWeightedWaveletMatrix<
    T, W, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE
> wm(values, weights)
```

初期状態のバージョンは `0`。
`MAX_VERSION` 回まで更新できる。

# 更新

```cpp
int version = wm.set(k, value, weight)
int version = wm.set_value(k, value)
int version = wm.set_weight(k, weight)
```

最新版を更新し、新しいバージョン番号を返す。

# バージョン

```cpp
int count = wm.versions()
int version = wm.latest_version()
```

# クエリ

```cpp
T value = wm.access(version, k)
W weight = wm.weight(version, k)
int count = wm.rank(version, value, l, r)
T value = wm.kth_smallest(version, l, r, k)
T value = wm.kth_largest(version, l, r, k)
int count = wm.range_freq(version, l, r, lower, upper)
W sum = wm.sum(version, l, r)
W sum = wm.range_sum(version, l, r, lower, upper)
W sum = wm.sum_k_smallest(version, l, r, k)
W sum = wm.sum_k_largest(version, l, r, k)
std::optional<T> value = wm.prev_value(version, l, r, upper)
std::optional<T> value = wm.next_value(version, l, r, lower)
```

区間は半開区間 `[l, r)`。
`range_sum` は値が `[lower, upper)` に入る要素の重みの総和を返す。

# 計算量

`B = BLOCK_SIZE`、区間が触れるブロック数を `C`、バージョン数を `V` とする。

- `set`, `set_value`, `set_weight`: `O(B log B + log V)`
- `access`, `weight`: `O(log V)`
- `rank`, `range_freq`, `sum`, `range_sum`: `O(B + C(log B + log V))`
- `kth_smallest`, `kth_largest`: `O(BIT_WIDTH * (B + C(log B + log V)))`
- 追加メモリ: `O(N + V B + V log V)`

更新では変更されたブロックだけを複製する。
