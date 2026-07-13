---
title: Segment Tree Beats
documentation_of: ../src/structure/segtree/segtree_beats.hpp
---

数列に対する区間chmin、区間chmax、区間加算と、区間和・最小値・最大値を扱う。

indexはすべて0-indexed。区間は半開区間 `[l,r)`。

# テンプレート引数

```cpp
SegmentTreeBeats<T, MAX_SIZE>
```

- 要素と区間和の型 `T`
- 配列長の上限 `MAX_SIZE`

`T` は比較と四則演算ができ、計算途中の値を保持できる型とする。

オブジェクト自体のcopy/moveはできない。大きい `MAX_SIZE` ではstatic領域かheapに置く。

# コンストラクタ

```cpp
(1) SegmentTreeBeats(int n = MAX_SIZE)
(2) SegmentTreeBeats(int n, const T& value)
(3) SegmentTreeBeats(const vector<T>& v)
(4) SegmentTreeBeats(const array<T, N>& v)
```

1. 長さ `n`、初期値 `T{}` で初期化する。
2. 長さ `n`、初期値 `value` で初期化する。
3. `v` で初期化する。
4. `v` で初期化する。

## 時間計算量

- $O(\mathrm{MAX\_SIZE})$

# range_chmin

```cpp
void range_chmin(int l, int r, const T& x)
```

各 $i \in [l,r)$ について、$a_i \leftarrow \min(a_i,x)$ とする。

## 時間計算量

- amortized $O(\log^2 n)$

# range_chmax

```cpp
void range_chmax(int l, int r, const T& x)
```

各 $i \in [l,r)$ について、$a_i \leftarrow \max(a_i,x)$ とする。

## 時間計算量

- amortized $O(\log^2 n)$

# range_add

```cpp
void range_add(int l, int r, const T& x)
```

各 $i \in [l,r)$ に `x` を加える。

## 時間計算量

- $O(\log n)$

# range_sum

```cpp
T range_sum(int l, int r)
```

$\sum_{i=l}^{r-1} a_i$ を返す。

## 時間計算量

- $O(\log n)$

# range_min

```cpp
T range_min(int l, int r)
```

$[l,r)$ の最小値を返す。`l < r` が必要。

## 時間計算量

- $O(\log n)$

# range_max

```cpp
T range_max(int l, int r)
```

$[l,r)$ の最大値を返す。`l < r` が必要。

## 時間計算量

- $O(\log n)$

# set

```cpp
void set(int k, const T& x)
```

`k` 番目を `x` に変更する。

## 時間計算量

- $O(\log n)$

# get

```cpp
T get(int k)
```

`k` 番目を返す。

## 時間計算量

- $O(\log n)$

# all_sum

```cpp
T all_sum() const
```

$[0,n)$ の和を返す。`n = 0` のときは `T{}`。

## 時間計算量

- $O(1)$

# all_min

```cpp
T all_min() const
```

$[0,n)$ の最小値を返す。`n > 0` が必要。

## 時間計算量

- $O(1)$

# all_max

```cpp
T all_max() const
```

$[0,n)$ の最大値を返す。`n > 0` が必要。

## 時間計算量

- $O(1)$

# size

```cpp
int size() const
```

配列長を返す。

## 時間計算量

- $O(1)$

# 引数・容量・例外

実際の列長を (n) とする。点は `0 <= k < n`、全range操作は
`0 <= l <= r <= n` の半開区間 `[l,r)` で指定する。`range_chmin/chmax/add` は
区間更新、`range_sum/min/max` は値を返す。`all_sum/min/max` は `[0,n)` を返す。
列長が `MAX_SIZE` を超える場合、点・区間が不正な場合は `runtime_error`。
各APIの計算量は上記の通り。
