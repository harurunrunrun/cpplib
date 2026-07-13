---
title: Sqrt Tree
documentation_of: ../src/structure/other/sqrt_tree.hpp
---

静的semigroup列の区間積。演算は結合的であればよく、単位元と可換性を要求しない。

# テンプレート引数

~~~cpp
SqrtTree<Semigroup, MAX_SIZE>
~~~

`Semigroup` は次を持つ。

~~~cpp
using S = ...;
S op(S a, S b);
~~~

`op(a,b)` は `a` の区間の後ろに `b` の区間を連結する順序で呼ばれる。

# 構築・操作

~~~cpp
SqrtTree()
SqrtTree(const vector<S>& values)
void build(const vector<S>& values)
int size() const
bool empty() const
S get(int index) const
S prod(int l, int r) const
S all_prod() const
~~~

`prod` は非空半開区間 `[l,r)` を扱う。空列の `all_prod` は例外を送出する。

# 計算量

$N$ を列長、$L=O(\log\log(N+2))$ をlayer数とし、semigroup演算を $O(1)$ とする。

- default constructor: 固定長arena初期化 $O(\mathrm{MAX\_SIZE}\log\log(\mathrm{MAX\_SIZE}+2))$
- vector constructor: 固定長arena初期化とbuildを含めて $O(\mathrm{MAX\_SIZE}\log\log(\mathrm{MAX\_SIZE}+2)+NL)$
- `build`: $O(NL)$
- `size`, `empty`, `get`, `prod`, `all_prod`: $O(1)$

各layerでblock内prefix/suffixとblock間積を前計算し、queryでは異なる最上位bitに対応するlayerをtableから直接選ぶ。

# 空間計算量

- 固定arena: $O(\mathrm{MAX\_SIZE}\log\log(\mathrm{MAX\_SIZE}+2))$
- queryの追加領域: $O(1)$
