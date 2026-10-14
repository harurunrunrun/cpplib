---
title: Wide Segtree (ワイドセグメント木) [point_add_range_sum]
documentation_of: ../../../../src/structure/segtree/basic/widesegtree.hpp
---

広い分岐数を持つ動的セグメント木。

値は `long long`、演算は和。

## テンプレート引数

```cpp
Wide_Segtree<MAX_SIZE>
```

`[0, MAX_SIZE)` を扱う。

## point update

```cpp
void set(idx, x)
void add(idx, x)
long long get(idx)
```

## 時間計算量

- `set`, `add`, `get`: $O(\log_B \mathtt{MAX\_SIZE})$

## range product

```cpp
long long prod(l, r)
long long all_prod()
```

`prod(l, r)` は半開区間 `[l,r)` の和を返す。

## 時間計算量

- `prod`: $O(\log_B \mathtt{MAX\_SIZE})$
- `all_prod`: $O(1)$

## 構築とcopy

- default constructor: $O(1)$
- copy constructor, copy assignment: $O(U)$

$B$ は実行環境で選ばれる分岐数、$U$ は確保済みnode数。各point updateが新規確保するnode数は $O(\log_B \mathtt{MAX\_SIZE})$。

## 注意点

`set(idx,x)` は1点を置換し、`add(idx,x)` は加算する。`get` は1点、
`prod(l,r)` は半開区間 `[l,r)`、`all_prod` は全域の和を返す。
点・区間が `[0,MAX_SIZE)` 外なら `runtime_error`、確保失敗は `bad_alloc`。
各APIの計算量は上記の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`Wide_Segtree<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
