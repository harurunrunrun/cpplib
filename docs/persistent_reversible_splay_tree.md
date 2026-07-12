---
title: Persistent Reversible Splay Tree
documentation_of: ../src/structure/bbst/persistent_reversible_splay_tree.hpp
---

永続列。区間反転、区間積を行う。

# テンプレート引数

```cpp
PersistentReversibleSplayTree<Monoid, MAX_NODE, MAX_VERSION>
```

`Monoid` は `S op(S, S)` と `S e()` を持つ。

# コンストラクタ

```cpp
PersistentReversibleSplayTree()
PersistentReversibleSplayTree(values)
```

# 更新

```cpp
int insert(p, x, version)
int push_front(x, version)
int push_back(x, version)
int erase(p, version)
int set(p, x, version)
int reverse(l, r, version)
```

更新は新しい version を返す。

# 取得

```cpp
S get(p, version)
S prod(l, r, version)
S all_prod(version)
vector<S> to_vector(version)
```

半開区間 `[l, r)` を扱う。

## 時間計算量

- expected $O(\log N)$
