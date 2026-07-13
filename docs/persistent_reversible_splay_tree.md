---
title: Persistent Reversible Splay Tree
documentation_of: ../src/structure/bbst/persistent_reversible_splay_tree.hpp
---

path copying を行う完全永続 implicit splay tree。列の挿入、削除、区間反転、区間積を扱う。更新元の version は変更しない。

# テンプレート引数

```cpp
PersistentReversibleSplayTree<Monoid, MAX_NODE, MAX_VERSION>
```

`Monoid` は次を持つ。

```cpp
using S = ...;
S op(S a, S b);
S e();
```

`op(a, b)` は `a` の後ろに `b` を連結した順序で呼ばれる。可換である必要はない。

`MAX_NODE` は初期列と全versionの path copy が共有する node 数、`MAX_VERSION` は version 0 を含む version 数の上限である。

# コンストラクタ

```cpp
PersistentReversibleSplayTree()
PersistentReversibleSplayTree(const vector<S>& values)
```

空、または `values` を初期列とする version 0 を作る。

# 更新

```cpp
int insert(int p, const S& x, int version = 0)
int push_front(const S& x, int version = 0)
int push_back(const S& x, int version = 0)
int erase(int p, int version = 0)
int set(int p, const S& x, int version = 0)
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

取得では splay せず、新しい version も作らない。

# 計算量

対象versionの木の高さを $H$、列長を $N$ とする。$H$ は worst-case $N$。モノイド演算は $O(1)$ とする。

- default constructor: 固定arena初期化 $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- vector constructor: 上記に加え長さ $N$ の平衡なversion 0構築 $O(N)$
- `versions`, `size`, `empty`, `all_prod`: $O(1)$
- `insert`, `push_front`, `push_back`, `erase`, `set`, `reverse`: $O(H)$、新規version $O(1)$、path-copy node $O(H)$
- `get`, `prod`: $O(H)$。木を変形せずversionも生成しない
- `to_vector`: 出力要素数 $N$ に対して $O(N)$

更新は内部の定数回のsplit/mergeを含む。直前versionだけを更新する一本道では償却 $O(\log(N+1))$。古いversionから分岐する操作列にはこの償却評価を共有できず、各更新の保証は $O(H)$。

## 空間計算量

- 固定arenaとroot table: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `to_vector` の戻り値と再帰stack: $O(N)$
