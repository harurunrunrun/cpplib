---
title: Persistent Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/persistent_wavelet_matrix.hpp
---

完全永続の点更新wavelet matrix。
任意のバージョンから分岐して `set` できる。

# テンプレート引数

```cpp
PersistentWaveletMatrix<T, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE>
```

# 更新

```cpp
int nv = wm.set(version, k, value)
int nv = wm.fork(version)
```

# クエリ

```cpp
wm.access(version, k)
wm.rank(version, value, l, r)
wm.select(version, value, k)
wm.kth_smallest(version, l, r, k)
wm.kth_largest(version, l, r, k)
wm.range_freq(version, l, r, upper)
wm.range_freq(version, l, r, lower, upper)
wm.prev_value(version, l, r, upper)
wm.next_value(version, l, r, lower)
```

# 時間計算量

$B = BLOCK\_SIZE$、$M = \lceil N / B \rceil$ とする。
区間が触れるブロック数を $C$ とする。

- `set`: $O(B\log B + \log M)$
- `fork`: $O(1)$
- `access`: $O(\log M)$
- `rank`, `range_freq`: $O(B + C(\log B + \log M))$
- `kth_smallest`: $O(BIT\_WIDTH \cdot (B + C(\log B + \log M)))$

ブロック参照は永続セグメント木で管理する。
`set` は変更ブロックと参照木の根から葉までだけを複製し、
`fork` は参照木の根を共有する。

更新を行ったバージョン数を $U$、全バージョン数を $V$ とした追加メモリは
$O(N + U(B + \log M) + V)$。
