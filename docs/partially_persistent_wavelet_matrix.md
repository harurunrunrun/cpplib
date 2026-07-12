---
title: Partially Persistent Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix.hpp
---

点更新できる部分永続 Wavelet Matrix。
更新対象は最新バージョンに限る。過去のバージョンはクエリで参照できる。

# コンストラクタ

```cpp
PartiallyPersistentWaveletMatrix<T, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE> wm(a)
```

初期状態のバージョンは `0`。
`MAX_VERSION` 回まで更新できる。

# 更新

```cpp
int version = wm.set(k, value)
```

最新版の `a[k]` を `value` に変更し、新しいバージョン番号を返す。

# バージョン

```cpp
int count = wm.versions()
int version = wm.latest_version()
```

# クエリ

```cpp
T value = wm.access(version, k)
int count = wm.rank(version, value, l, r)
int index = wm.select(version, value, k)
T value = wm.kth_smallest(version, l, r, k)
T value = wm.kth_largest(version, l, r, k)
int count = wm.range_freq(version, l, r, upper)
int count = wm.range_freq(version, l, r, lower, upper)
std::optional<T> value = wm.prev_value(version, l, r, upper)
std::optional<T> value = wm.next_value(version, l, r, lower)
```

区間は半開区間 `[l, r)`。
`select` は `value` の `k` 番目の位置を返し、存在しなければ `size()` を返す。

# 計算量

`B = BLOCK_SIZE`、区間が触れるブロック数を `C`、バージョン数を `V` とする。

- `set`: `O(B log B + log V)`
- `access`: `O(log V)`
- `rank`, `range_freq`: `O(B + C(log B + log V))`
- `kth_smallest`, `kth_largest`: `O(BIT_WIDTH * (B + C(log B + log V)))`
- 追加メモリ: `O(N + V B + V log V)`

更新では変更されたブロックだけを複製する。
