---
title: Persistent Lazy Weighted Balanced Tree
documentation_of: ../src/structure/bbst/persistent_lazy_weighted_balanced_tree.hpp
---

重みを部分木サイズとする平衡条件を保ち、path copying を行う完全永続 implicit weight-balanced tree。列の挿入、削除、区間反転、区間作用、区間積を扱う。更新元の version は変更しない。

# テンプレート引数

```cpp
PersistentLazyWeightedBalancedTree<MonoidActLen, MAX_NODE, MAX_VERSION>
```

`MonoidActLen` は次を持つ。

```cpp
using S = ...;
using T = ...;
S op(S a, long long len_a, S b, long long len_b);
S e();
S mapping(T f, S x, long long len);
T composition(T f, T g);
T id();
```

`op(a, len_a, b, len_b)` は `a` の後ろに `b` を連結した順序で呼ばれる。可換である必要はない。`mapping(f, x, len)` は区間中の各要素へ同じ作用 `f` を適用した集約値を返す。`composition(f, g)` は `g` の後に `f` を適用する合成を返す。作用は連結と反転後の集約値の双方に適用できる必要がある。

`MAX_NODE` は初期列と全versionの path copy が共有する node 数、`MAX_VERSION` は version 0 を含む version 数の上限である。

# コンストラクタ

```cpp
PersistentLazyWeightedBalancedTree()
PersistentLazyWeightedBalancedTree(const vector<S>& values)
```

空、または `values` を初期列とする version 0 を作る。

# 更新

```cpp
int insert(int p, const S& x, int version = 0)
int erase(int p, int version = 0)
int set(int p, const S& x, int version = 0)
int apply(int l, int r, const T& f, int version = 0)
int reverse(int l, int r, int version = 0)
```

`version` から新しい version を作り、その番号を返す。区間は半開区間 `[l, r)` である。

node または version の容量を超えた場合は例外を送出し、version 数、使用 node 数、既存versionを更新前の状態に保つ。

# 取得

```cpp
int versions() const
int size(int version = 0) const
bool empty(int version = 0) const
S get(int p, int version = 0) const
S prod(int l, int r, int version = 0) const
S all_prod(int version = 0) const
vector<S> to_vector(int version = 0) const
```

取得は木を変更せず、新しい version も作らない。

# 計算量

列長を N とする。

- 構築: O(N) 時間、O(N) node
- insert, erase, set, apply, reverse: O(log N) 時間、O(log N) node
- get, prod: O(log N) 時間
- size, empty, all_prod, versions: O(1) 時間
- to_vector: O(N) 時間

平衡化は部分木サイズを用い、乱数には依存しない。