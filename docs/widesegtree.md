---
title: Wide Segtree
documentation_of: ../src/structure/segtree/widesegtree.hpp
---

広い分岐数を持つ動的セグメント木。

値は `long long`、演算は和。

# テンプレート引数

```cpp
Wide_Segtree<MAX_SIZE>
```

`[0, MAX_SIZE)` を扱う。

# point update

```cpp
void set(idx, x)
void add(idx, x)
long long get(idx)
```

## 時間計算量

- `set`, `add`, `get`: $O(\log_B \mathtt{MAX\_SIZE})$

# range product

```cpp
long long prod(l, r)
long long all_prod()
```

`prod(l, r)` は半開区間 `[l,r)` の和を返す。

## 時間計算量

- `prod`: $O(\log_B \mathtt{MAX\_SIZE})$
- `all_prod`: $O(1)$

# 構築とcopy

- default constructor: $O(1)$
- copy constructor, copy assignment: $O(U)$

$B$ は実行環境で選ばれる分岐数、$U$ は確保済みnode数。各point updateが新規確保するnode数は $O(\log_B \mathtt{MAX\_SIZE})$。

# API契約

`set(idx,x)` は1点を置換し、`add(idx,x)` は加算する。`get` は1点、
`prod(l,r)` は半開区間 `[l,r)`、`all_prod` は全域の和を返す。
点・区間が `[0,MAX_SIZE)` 外なら `runtime_error`、確保失敗は `bad_alloc`。
各APIの計算量は上記の通り。
