---
title: Dynamic Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/dynamic_wavelet_matrix.hpp
---

点更新できる整数列。API は `WaveletMatrix` に近い。
内部はブロックごとにソート列を持つ。

# テンプレート引数

```cpp
DynamicWaveletMatrix<T, MAX_SIZE, BIT_WIDTH, BLOCK_SIZE>
```

- 整数型 `T`
- 配列長の上限 `MAX_SIZE`
- 使用するbit数 `BIT_WIDTH`
- ブロックサイズ `BLOCK_SIZE`

符号付き整数では `BIT_WIDTH` を省略する。

# set

```cpp
wm.set(k, value)
```

`a[k] = value` に更新する。

## 時間計算量

- $O(BLOCK\_SIZE \log BLOCK\_SIZE)$

# 要素取得

```cpp
wm.access(k)
wm[k]
```

## 時間計算量

- $O(1)$

# rank / select

```cpp
wm.rank(value, r)
wm.rank(value, l, r)
wm.select(value, k)
```

`rank` は区間内の `value` の個数を返す。
`select` は `value` が `k` 回目に現れる位置を返し、存在しない場合は `size()` を返す。

## 時間計算量

- `rank`: $O(BLOCK\_SIZE + \frac{N}{BLOCK\_SIZE}\log BLOCK\_SIZE)$
- `select`: `rank` に追加で二分探索

# k番目

```cpp
wm.kth_smallest(l, r, k)
wm.kth_largest(l, r, k)
```

区間 $[l,r)$ の `k` 番目に小さい値または大きい値を返す。`k` は0-indexed。

## 時間計算量

- $O(BIT\_WIDTH \cdot (BLOCK\_SIZE + \frac{N}{BLOCK\_SIZE}\log BLOCK\_SIZE))$

# 頻度

```cpp
wm.range_freq(l, r, upper)
wm.range_freq(l, r, lower, upper)
```

区間 $[l,r)$ にある `upper` 未満、または `[lower, upper)` の値の個数を返す。

## 時間計算量

- $O(BLOCK\_SIZE + \frac{N}{BLOCK\_SIZE}\log BLOCK\_SIZE)$

# 前後の値

```cpp
wm.prev_value(l, r, upper)
wm.next_value(l, r, lower)
```

区間内の `upper` 未満の最大値、または `lower` 以上の最小値を返す。存在しない場合は `nullopt`。

## 時間計算量

- `kth_smallest` と同じ
