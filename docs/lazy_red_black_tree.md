---
title: Lazy Red Black Tree
documentation_of: ../src/structure/bbst/lazy_red_black_tree.hpp
---

Red Black Tree によるキー付き列。キーの半開区間に対して、値の作用、値の積を行う。

# テンプレート引数

```cpp
LazyRedBlackTree<Key, MonoidActLen, MAX_SIZE, Compare>
```

- キーの型 `Key`
- 作用付きモノイド `MonoidActLen`
- 同時に保持する要素数の上限 `MAX_SIZE`
- 比較関数 `Compare`

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

# insert / erase

```cpp
bool insert(const Key& key, const S& value)
bool erase(const Key& key)
bool contains(const Key& key)
```

`insert` は同じキーが存在する場合 `false` を返す。

`erase` はキーが存在しない場合 `false` を返す。

## 時間計算量

- $O(\log N)$

# get / set

```cpp
optional<S> get(const Key& key)
bool set(const Key& key, const S& value)
```

`get` はキーが存在しない場合 `nullopt` を返す。

`set` はキーが存在しない場合 `false` を返す。

## 時間計算量

- $O(\log N)$

# 区間操作

```cpp
void apply(const Key& l, const Key& r, const T& f)
S prod(const Key& l, const Key& r)
void all_apply(const T& f)
S all_prod()
```

キーの半開区間 `[l,r)` を扱う。

## 時間計算量

- $O(\log N)$

# 順序

```cpp
int order_of_key(const Key& key)
optional<Key> kth_key(int k)
```

## 時間計算量

- $O(\log N)$

# API別計算量

$N$ を現在の要素数とし、比較・モノイド演算・作用を $O(1)$ とする。

- `LazyRedBlackTree(compare)`: $O(\mathrm{MAX\_SIZE})$。固定長storageを確保・初期化する
- `size`, `empty`, `clear`, `all_apply`, `all_prod`: $O(1)$
- `contains`, `insert`, `erase`, `get`, `set`: worst-case $O(\log(N+1))$
- `apply`, `prod`: worst-case $O(\log(N+1))$
- `order_of_key`, `kth_key`: worst-case $O(\log(N+1))$

キー区間操作は境界に至るpathだけを展開し、完全に含まれる部分木はlazy tagまたはaggregateでまとめて処理する。

## 空間計算量

- object全体: $O(\mathrm{MAX\_SIZE})$
- setの固定長祖先array: $O(\mathrm{MAX\_SIZE})$（使用要素数は $O(\log(N+1))$）
- insert / erase / apply / prodの再帰stack: $O(\log(N+1))$
- その他の操作の追加領域: $O(1)$

# 前提・例外・容量

- `Compare`はstrict weak ordering。同値キーは1個だけ保持する。
- 新規insertで要素数が`MAX_SIZE`を超える場合は例外。
- `apply(l,r,f)`と`prod(l,r)`は`Compare(r,l)==false`を要求し、逆転時は例外。
- `get`は不存在なら`nullopt`、`set`と`erase`は不存在なら`false`。
- `kth_key`は範囲外なら`nullopt`。`clear`は固定arenaを再利用する。
