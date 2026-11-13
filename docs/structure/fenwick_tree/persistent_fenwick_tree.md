---
title: Persistent Fenwick Tree (永続Fenwick木)
documentation_of: ../../../src/structure/fenwick_tree/persistent_fenwick_tree.hpp
---

任意の過去のバージョンから分岐して更新できる、完全永続な Fenwick Tree。Fenwick Tree の各ノードの値を永続セグメント木に格納する。

## API

```cpp
template<class T> class PersistentFenwickTree;
```

- `explicit PersistentFenwickTree(int n)`: 長さ $n$ の零列をバージョン $0$ として構築する。
- `int size() const`、`int version_count() const`: 列の長さ、バージョン数を返す。
- `int add(int base_version, int position, const T& delta)`: バージョン `base_version` の `position` 番目に `delta` を加えた新しいバージョンを作り、その番号を返す。
- `int add(int position, const T& delta)`: 最新のバージョンをもとに加算する簡便版。
- `T prefix_sum(int version, int right) const`、`T sum(int version, int left, int right) const`: 指定したバージョンの $[0, \mathrm{right})$、$[\mathrm{left}, \mathrm{right})$ の和を返す。

## 時間計算量

- 構築: $O(1)$
- `add`: $O(\log^2 N)$。ノードを $O(\log^2 N)$ 個追加する
- `prefix_sum`、`sum`: $O(\log^2 N)$
- `size`、`version_count`: $O(1)$

## 空間計算量

$O(V + U\log^2 N)$（$V$ はバージョン数、$U$ は `add` の回数）。

## 注意点

- 更新後も、もとにしたバージョンを含む既存のすべてのバージョンは変化しない。
- `T` には `T{}`、加算、減算が使える型を用いる。
- 負の長さは `std::invalid_argument` を、長さ `INT_MAX` は内部の 1-indexed の境界を表現できないため `std::length_error` を送出する。
- 不正なバージョン・位置・区間は `std::out_of_range` を送出する。
