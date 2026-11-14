---
title: Persistent Segment Tree (完全永続セグメント木)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_segtree.hpp
---

任意の過去の状態を残したまま、1 点更新を行った新しい状態を作るセグメント木。

状態は 0 から始まる version 番号で指定し、version 0 が初期状態である。

## テンプレート引数

```cpp
PersistentSegtree<Monoid, MAX_SIZE, MAX_VERSION>
```

- `Monoid`: モノイド
- `MAX_SIZE`: 配列長の上限
- `MAX_VERSION`: version 0 から追加できる version 数の上限

容量（`MAX_SIZE`, `MAX_VERSION`）を両方省略すると可変長版になり、`PersistentSegtree<Monoid> tree(n);` のように長さを実行時に与える。ノードと version は使用量に応じて `std::vector` を拡張する。

オブジェクト自体のコピー・ムーブはできない。

## コンストラクタ

```cpp
(1) PersistentSegtree(int n = MAX_SIZE)  // 可変長版の既定値は 0
(2) PersistentSegtree(const vector<S>& v)
(3) PersistentSegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する（$N \le \texttt{MAX_SIZE}$ を `static_assert` で確認する）。

## 時間計算量

- $O(\texttt{MAX_SIZE}+\texttt{MAX_VERSION}\log \texttt{MAX_SIZE})$（可変長版は $O(n)$）

## set

```cpp
int set(int version, int k, const S& x)
```

`version` の `k` 番目を `x` に変更した新しい version を作り、その番号を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## fork

```cpp
int fork(int version)
```

`version` と同じ内容の新しい version を作り、その番号を返す。

## 時間計算量

- $O(1)$

## get

```cpp
S get(int version, int k) const
```

`version` の `k` 番目を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## prod

```cpp
S prod(int version, int l, int r) const
```

`version` の区間 $[l, r)$ の総積を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## all_prod

```cpp
S all_prod(int version) const
```

`version` の全要素の総積を返す。

## 時間計算量

- $O(1)$

## max_right

```cpp
int max_right(int version, int l, auto f, auto&&... args) const
```

`f(Monoid.e(), args...)` が `true` であるとき、`f(prod(version, l, r), args...)` が `true` となる最大の `r` を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## min_left

```cpp
int min_left(int version, int r, auto f, auto&&... args) const
```

`f(Monoid.e(), args...)` が `true` であるとき、`f(prod(version, l, r), args...)` が `true` となる最小の `l` を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## size

```cpp
int size() const
```

配列長を返す。

## 時間計算量

- $O(1)$

## versions

```cpp
int versions() const
```

現在存在する version 数を返す。

## 時間計算量

- $O(1)$

## latest_version

```cpp
int latest_version() const
```

最後に作った version の番号を返す。

## 時間計算量

- $O(1)$

## 注意点

- version 0 が初期状態で、`0 <= version < versions()` の version だけを参照できる。
- 更新と `fork` は指定した version を変更せず、新しい version 番号を返す。`latest_version()` は `versions() - 1` である。
- 点は実際の列内、区間は半開区間 $[l, r)$ で指定する。
- 境界探索の述語は単調で、単位元に対して `true` を返す必要がある。
- 不正な version・点・区間、述語が単位元で `false`、version の容量超過では `std::runtime_error` を送出する。可変長版で $n \gt 2^{28}$ の場合は `std::length_error` になる。
- 更新が失敗した場合、version 数と使用ノード数は更新前に戻る。
