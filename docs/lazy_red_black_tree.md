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
