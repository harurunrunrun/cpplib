---
title: Segment Tree Beats (セグメント木Beats) [range_add_range_min] [range_chmin_chmax_add_range_sum]
documentation_of: ../../../../src/structure/segtree/basic/segtree_beats.hpp
---

数列に対する区間chmin、区間chmax、区間加算と、区間和・最小値・最大値を扱う。

indexはすべて0-indexed。区間は半開区間 `[l,r)`。

## テンプレート引数

```cpp
SegmentTreeBeats<T, MAX_SIZE>
```

- 要素と区間和の型 `T`
- 配列長の上限 `MAX_SIZE`

`T` は比較と四則演算ができ、計算途中の値を保持できる型とする。

オブジェクト自体のcopy/moveはできない。大きい `MAX_SIZE` ではstatic領域かheapに置く。

## コンストラクタ

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

## range_chmin

```cpp
void range_chmin(int l, int r, const T& x)
```

各 $i \in [l,r)$ について、$a_i \leftarrow \min(a_i,x)$ とする。

## 時間計算量

- amortized $O(\log^2 n)$

## range_chmax

```cpp
void range_chmax(int l, int r, const T& x)
```

各 $i \in [l,r)$ について、$a_i \leftarrow \max(a_i,x)$ とする。

## 時間計算量

- amortized $O(\log^2 n)$

## range_add

```cpp
void range_add(int l, int r, const T& x)
```

各 $i \in [l,r)$ に `x` を加える。

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

$[l,r)$ の最小値を返す。`l < r` が必要。

## 時間計算量

- $O(\log n)$

## range_max

```cpp
T range_max(int l, int r)
```

$[l,r)$ の最大値を返す。`l < r` が必要。

## 時間計算量

- $O(\log n)$

## set

```cpp
void set(int k, const T& x)
```

`k` 番目を `x` に変更する。

## 時間計算量

- $O(\log n)$

## get

```cpp
T get(int k)
```

`k` 番目を返す。

## 時間計算量

- $O(\log n)$

## all_sum

```cpp
T all_sum() const
```

$[0,n)$ の和を返す。`n = 0` のときは `T{}`。

## 時間計算量

- $O(1)$

## all_min

```cpp
T all_min() const
```

$[0,n)$ の最小値を返す。`n > 0` が必要。

## 時間計算量

- $O(1)$

## all_max

```cpp
T all_max() const
```

$[0,n)$ の最大値を返す。`n > 0` が必要。

## 時間計算量

- $O(1)$

## size

```cpp
int size() const
```

配列長を返す。

## 時間計算量

- $O(1)$

## 注意点

実際の列長を (n) とする。点は `0 <= k < n`、全range操作は
`0 <= l <= r <= n` の半開区間 `[l,r)` で指定する。`range_chmin/chmax/add` は
区間更新、`range_sum/min/max` は値を返す。`all_sum/min/max` は `[0,n)` を返す。
列長が `MAX_SIZE` を超える場合、点・区間が不正な場合は `runtime_error`。
各APIの計算量は上記の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`SegmentTreeBeats<T> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
