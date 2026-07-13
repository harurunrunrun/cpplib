---
title: Persistent Lazy Reversible Splay Tree (完全永続遅延反転可能スプレー木)
documentation_of: ../src/structure/bbst/persistent_lazy_reversible_splay_tree.hpp
---

path copying を行う完全永続 implicit splay tree。列の挿入、削除、区間反転、区間作用、区間積を扱う。更新元の version は変更しない。

# テンプレート引数

```cpp
PersistentLazyReversibleSplayTree<MonoidActLen, MAX_NODE, MAX_VERSION>
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
PersistentLazyReversibleSplayTree()
PersistentLazyReversibleSplayTree(const vector<S>& values)
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

取得では splay せず、新しい version も作らない。

# 時間計算量

対象versionの木の高さを $H$、列長を $N$ とする。$H$ は worst-case $N$。モノイド演算・作用は $O(1)$ とする。

- default constructor: 固定arena初期化 $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- vector constructor: 上記に加え長さ $N$ の平衡なversion 0構築 $O(N)$
- `versions`, `size`, `empty`, `all_prod`: $O(1)$
- `insert`, `erase`, `set`, `apply`, `reverse`: $O(H)$、新規version $O(1)$、path-copy node $O(H)$
- `get`, `prod`: $O(H)$。木を変形せずversionも生成しない
- `to_vector`: 出力要素数 $N$ に対して $O(N)$

更新は内部の定数回のsplit/mergeを含む。直前versionだけを更新する一本道では償却 $O(\log(N+1))$。古いversionから分岐する操作列にはこの償却評価を共有できず、各更新の保証は $O(H)$。

## 空間計算量

- 固定arenaとroot table: $O(\mathrm{MAX\_NODE}+\mathrm{MAX\_VERSION})$
- `to_vector` の戻り値と再帰stack: $O(N)$

## 注意点

- version、insert位置、点index、半開区間は対象versionのsize内でなければならない。
- 範囲違反、version/node容量超過は例外で、既存versionを変えない。
- updateは内容が変わらない場合もversionを1個作り、queryは容量を消費しない。
- `prod`は左から右の順で合成し、空区間では単位元を返す。
- `composition(f,g)`は先に`g`、後に`f`。
- copyは禁止、moveは可能。
