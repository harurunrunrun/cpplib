---
title: Treap
documentation_of: ../src/structure/tree/treap.hpp
---

乱択平衡二分探索木による集合。

# テンプレート引数

```cpp
TreapSet<Key, MAX_SIZE, Compare>
```

- キーの型 `Key`
- 同時に保持する要素数の上限 `MAX_SIZE`
- 比較関数 `Compare`

同じキーは 1 個だけ保持する。

# コンストラクタ

```cpp
TreapSet(uint64_t seed = ..., const Compare& compare = Compare())
```

優先度の生成に `seed` を使う。

## 時間計算量

- $O(\mathtt{MAX\_SIZE})$

固定容量のkey・node領域を構築する。

# insert / erase / contains

```cpp
bool insert(const Key& x)
bool erase(const Key& x)
bool contains(const Key& x)
```

`insert` はすでに存在する場合 `false` を返す。

`erase` は存在しない場合 `false` を返す。

## 時間計算量

- expected $O(\log N)$

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

- expected $O(\log N)$

# 探索

```cpp
optional<Key> lower_bound(const Key& x)
optional<Key> upper_bound(const Key& x)
optional<Key> max_leq(const Key& x)
optional<Key> max_less(const Key& x)
```

存在しない場合 `nullopt` を返す。

## 時間計算量

- expected $O(\log N)$

# 状態情報

`size`, `empty`, `clear` は $O(1)$。
