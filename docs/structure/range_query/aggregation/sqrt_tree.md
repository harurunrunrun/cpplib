---
title: Sqrt Tree (平方根木)
documentation_of: ../../../../src/structure/range_query/aggregation/sqrt_tree.hpp
---

静的な半群の列に対して区間積を求める。演算は結合的であればよく、単位元と可換性は要求しない。

## テンプレート引数

~~~cpp
template<auto Semigroup, int MAX_SIZE = -1>
struct SqrtTree;
~~~

`MAX_SIZE` は列の長さの上限である。省略時（`-1`）は上限を設けず、実行時の長さに合わせて `std::vector` で領域を確保する。`Semigroup` は次を持つ。

~~~cpp
using S = ...;
S op(S a, S b);
~~~

`op(a, b)` は、`a` の区間の後ろに `b` の区間を連結する順序で呼ばれる。

## 構築・操作

~~~cpp
SqrtTree()
explicit SqrtTree(const vector<S>& values)
void build(const vector<S>& values)
int size() const
bool empty() const
S get(int index) const
S prod(int l, int r) const
S all_prod() const
~~~

`prod` は空でない半開区間 `[l, r)` の積を返す。空列に対する `all_prod` は例外を送出する。

## 時間計算量

$N$ を列の長さ、$L=O(\log\log(N+2))$ を層の数とし、半群の演算を $O(1)$ とする。

- デフォルトコンストラクタ: 固定長の領域の初期化に $O(\texttt{MAX_SIZE}\log\log(\texttt{MAX_SIZE}+2))$
- `vector` を受け取るコンストラクタ: 固定長の領域の初期化と `build` を合わせて $O(\texttt{MAX_SIZE}\log\log(\texttt{MAX_SIZE}+2)+NL)$
- `build`: $O(NL)$
- `size`, `empty`, `get`, `prod`, `all_prod`: $O(1)$

各層でブロック内の接頭辞積・接尾辞積とブロック間の積を前計算し、クエリでは両端の添字の最上位の異なるビットに対応する層を表から直接選ぶ。

`MAX_SIZE` を省略した場合、構築は $O(NL)$ である。

## 空間計算量

- 固定長の領域: $O(\texttt{MAX_SIZE}\log\log(\texttt{MAX_SIZE}+2))$（省略時は $O(NL)$）
- クエリの追加領域: $O(1)$

## 注意点

- `Semigroup.op` は結合的でなければならない。単位元と可換性は不要で、代数的な性質は実行時に検査しない。
- `0 <= size() <= MAX_SIZE`、`get` の添字は `[0, size())`、`prod` は空でない半開区間を要求する。空列への `all_prod` を含め、範囲違反では `std::runtime_error` を送出する。
