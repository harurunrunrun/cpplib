---
title: Splay Tree (スプレー木)
documentation_of: ../src/structure/bbst/splay_tree.hpp
---

Splay Tree による集合。

# テンプレート引数

```cpp
SplaySet<Key, MAX_SIZE, Compare>
```

- キーの型 `Key`
- 同時に保持する要素数の上限 `MAX_SIZE`
- 比較関数 `Compare`

同じキーは 1 個だけ保持する。

# コンストラクタ

```cpp
SplaySet(const Compare& compare = Compare())
```

## 時間計算量

- $O(\mathrm{MAX\_SIZE})$

# insert / erase / contains

```cpp
bool insert(const Key& x)
bool erase(const Key& x)
bool contains(const Key& x)
```

`insert` はすでに存在する場合 `false` を返す。

`erase` は存在しない場合 `false` を返す。

## 時間計算量

- amortized $O(\log N)$

# 順序

```cpp
int order_of_key(const Key& x)
int order_of_key_upper(const Key& x)
optional<Key> kth(int k)
```

`order_of_key(x)` は `x` 未満の要素数を返す。

`order_of_key_upper(x)` は `x` 以下の要素数を返す。

`kth(k)` は 0-indexed で `k` 番目の要素を返す。存在しない場合 `nullopt` を返す。

## 時間計算量

- amortized $O(\log N)$

# 探索

```cpp
optional<Key> lower_bound(const Key& x)
optional<Key> upper_bound(const Key& x)
optional<Key> max_leq(const Key& x)
optional<Key> max_less(const Key& x)
```

存在しない場合 `nullopt` を返す。

## 時間計算量

- amortized $O(\log N)$

# API別計算量

$N$ を現在の要素数とする。比較と `Key` のコピーは $O(1)$ とする。

- `SplaySet(compare)`: $O(\mathrm{MAX\_SIZE})$。固定長storageを確保・初期化する
- `size`, `empty`, `clear`: $O(1)$
- `contains`, `insert`, `erase`: worst-case $O(N)$、操作列に対して償却 $O(\log(N+1))$
- `order_of_key`, `order_of_key_upper`, `kth`: worst-case $O(N)$、償却 $O(\log(N+1))$
- `lower_bound`, `upper_bound`, `max_leq`, `max_less`: worst-case $O(N)$、償却 $O(\log(N+1))$

各探索も最後にnodeをsplayするため償却解析の対象になる。

## 空間計算量

- object全体: $O(\mathrm{MAX\_SIZE})$
- 各操作の追加領域: $O(1)$

# 前提・例外・容量

- `Compare`はstrict weak ordering。同値キーは1個だけ保持する。
- `insert`は既存キーなら`false`、要素数が`MAX_SIZE`の新規キーなら例外。
- `erase`は不存在なら`false`。`kth`と各境界探索は該当なしなら`nullopt`。
- `clear`は$O(1)$で固定node arenaを再利用する。
