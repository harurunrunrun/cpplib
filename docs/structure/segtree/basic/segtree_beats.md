---
title: Segment Tree Beats (セグメント木Beats) [range_add_range_min] [range_chmin_chmax_add_range_sum]
documentation_of: ../../../../src/structure/segtree/basic/segtree_beats.hpp
---

数列に対する区間 chmin、区間 chmax、区間加算と、区間和・区間最小値・区間最大値の取得を扱う。

添字はすべて 0-indexed で、区間は半開区間 `[l, r)` で指定する。

## テンプレート引数

```cpp
template<class T, int MAX_SIZE = -1>
struct SegmentTreeBeats;
```

- `T`: 要素と区間和の型
- `MAX_SIZE`: 配列の長さの上限

`T` は比較と四則演算ができ、計算途中の値を保持できる型とする。

オブジェクト自体のコピー・ムーブはできない。`MAX_SIZE` が大きい場合は、静的領域かヒープに置く。

## コンストラクタ

```cpp
(1) explicit SegmentTreeBeats(int n = MAX_SIZE)
(2) SegmentTreeBeats(int n, const T& value)
(3) explicit SegmentTreeBeats(const vector<T>& v)
(4) explicit SegmentTreeBeats(const array<T, N>& v)
```

1. 長さ `n`、初期値 `T{}` で初期化する。
2. 長さ `n`、初期値 `value` で初期化する。
3. `v` で初期化する。
4. 固定長配列 `v` で初期化する。

## 時間計算量

- $O(\texttt{MAX_SIZE})$

## range_chmin

```cpp
void range_chmin(int l, int r, const T& x)
```

各 $i \in [l, r)$ について、$a_i \leftarrow \min(a_i, x)$ とする。

## 時間計算量

- ならし $O(\log^2 n)$

## range_chmax

```cpp
void range_chmax(int l, int r, const T& x)
```

各 $i \in [l, r)$ について、$a_i \leftarrow \max(a_i, x)$ とする。

## 時間計算量

- ならし $O(\log^2 n)$

## range_add

```cpp
void range_add(int l, int r, const T& x)
```

各 $i \in [l, r)$ に `x` を加える。

## 時間計算量

- $O(\log n)$

## range_sum

```cpp
T range_sum(int l, int r)
```

$\sum_{i=l}^{r-1} a_i$ を返す。

## 時間計算量

- $O(\log n)$

## range_min

```cpp
T range_min(int l, int r)
```

$[l, r)$ の最小値を返す。`l < r` が必要で、空区間では `std::runtime_error` を送出する。

## 時間計算量

- $O(\log n)$

## range_max

```cpp
T range_max(int l, int r)
```

$[l, r)$ の最大値を返す。`l < r` が必要で、空区間では `std::runtime_error` を送出する。

## 時間計算量

- $O(\log n)$

## set

```cpp
void set(int k, const T& x)
```

`k` 番目の要素を `x` に変更する。

## 時間計算量

- $O(\log n)$

## get

```cpp
T get(int k)
```

`k` 番目の要素を返す。

## 時間計算量

- $O(\log n)$

## all_sum

```cpp
T all_sum() const
```

$[0, n)$ の和を返す。`n = 0` のときは `T{}` を返す。

## 時間計算量

- $O(1)$

## all_min

```cpp
T all_min() const
```

$[0, n)$ の最小値を返す。`n > 0` が必要で、`n = 0` では `std::runtime_error` を送出する。

## 時間計算量

- $O(1)$

## all_max

```cpp
T all_max() const
```

$[0, n)$ の最大値を返す。`n > 0` が必要で、`n = 0` では `std::runtime_error` を送出する。

## 時間計算量

- $O(1)$

## size

```cpp
int size() const
```

配列の長さを返す。

## 時間計算量

- $O(1)$

## 注意点

- 実際の列の長さを $n$ とする。点は `0 <= k < n`、区間操作はすべて `0 <= l <= r <= n` の半開区間 `[l, r)` で指定する。
- 列の長さが `MAX_SIZE` を超える場合や、点・区間が不正な場合は `std::runtime_error` を送出する。
- 計算量の $n$ は、固定長版では `MAX_SIZE` に依存する木の大きさで決まる。

## 容量を省略する場合

`MAX_SIZE` のデフォルトは `-1` である。`SegmentTreeBeats<T> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使い、実行時の長さに合わせて領域を確保する（このときのコンストラクタ (1) のデフォルトは `n = 0`）。長さが負または $2^{29}$ を超える場合は `std::length_error` を送出する。非負の容量を指定する固定長版も利用できる。
