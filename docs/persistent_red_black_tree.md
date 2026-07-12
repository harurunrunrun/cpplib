---
title: Persistent Red Black Tree
documentation_of: ../src/structure/bbst/persistent_red_black_tree.hpp
---

path-copy left-leaning red-black tree による完全永続構造。

永続平衡二分探索木による集合。

同じキーは 1 個だけ保持する。更新は新しい version を返す。

# テンプレート引数

```cpp
PersistentRedBlackSet<Key, MAX_NODE, MAX_VERSION, Compare>
```

# コンストラクタ

```cpp
PersistentRedBlackSet set
```

version `0` は空集合。

# insert / erase / contains

```cpp
int set.insert(x, version)
int set.erase(x, version)
bool set.contains(x, version)
```

`insert` と `erase` は更新後の version を返す。

# 順序

```cpp
int set.order_of_key(x, version)
int set.order_of_key_upper(x, version)
optional<Key> set.kth(k, version)
```

# 探索

```cpp
optional<Key> set.lower_bound(x, version)
optional<Key> set.upper_bound(x, version)
optional<Key> set.max_leq(x, version)
optional<Key> set.max_less(x, version)
```

## 時間計算量

- insert, erase, search: worst-case $O(\log N)$
- to_vector: $O(N)$
