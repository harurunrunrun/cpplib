---
title: Persistent Lazy Weighted Balanced Tree
documentation_of: ../src/structure/bbst/persistent_lazy_weighted_balanced_tree.hpp
---

永続列。区間反転、区間作用、区間積を行う。

# テンプレート引数

```cpp
PersistentLazyWeightedBalancedTree<MonoidActLen, MAX_NODE, MAX_VERSION>
```

# 更新

```cpp
int insert(p, x, version)
int erase(p, version)
int set(p, x, version)
int apply(l, r, f, version)
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
