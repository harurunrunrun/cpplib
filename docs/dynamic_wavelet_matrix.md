---
title: Dynamic Wavelet Matrix (動的ウェーブレット行列)
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

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$D=\mathtt{BIT\_WIDTH}$ とし、長さ $L$ の区間について
$Q(L)=B+\lceil L/B\rceil\log B$ とおく。

- default constructor: 固定長配列の初期化に $O(\mathtt{MAX\_SIZE})$
- vector/array constructor: $O(\mathtt{MAX\_SIZE}+N\log B)$
- `size`, `access`, `operator[]`: $O(1)$
- `set`: $O(B\log B)$
- `rank`, `range_freq`: $O(Q(L))$
- `select`: $O(Q(N)\log N)$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: $O(DQ(L))$

## 注意点

default constructorは空列、vector/array constructorは入力列を構築する。`size` は列長、
`access/operator[]` は1点を返す。`set(k,value)` は1点を置換する。
点・出現番号・順序統計の `k` は0-indexed、列区間は半開区間 `[l,r)`、
値域は `[lower,upper)`。`select` は該当なしなら `size()`、
`prev_value/next_value` は `nullopt`。

列長容量、bit幅、点・区間・順序・値域の違反では `runtime_error`。
各公開APIの計算量は上記表の通り。
