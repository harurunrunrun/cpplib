---
title: Lazy Reversible Splay Tree (遅延反転可能スプレー木) [dynamic_sequence_range_affine_range_sum]
documentation_of: ../../../../src/structure/bbst/sequence/lazy_reversible_splay_tree.hpp
---

Splay 木による列。区間反転、区間作用、区間積を行う。

## テンプレート引数

```cpp
LazyReversibleSplayTree<MonoidActLen, MAX_SIZE = -1>
```

- `MonoidActLen`: 作用付きモノイド（区間長付き）のオブジェクト
- `MAX_SIZE`: 同時に保持する要素数の上限

`MAX_SIZE` を指定するとノード領域を固定確保する。省略（`-1`）するとノードを `std::vector` に必要に応じて確保する可変長版になる（伸長コストは償却 $O(1)$ で、各操作の計算量は変わらない）。

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

非可換モノイドにも対応する。

## コンストラクタ

```cpp
LazyReversibleSplayTree()
LazyReversibleSplayTree(const std::vector<S>& values)
```

## insert / erase / set / get

```cpp
void insert(int p, const S& x)
void push_front(const S& x)
void push_back(const S& x)
void erase(int p)
void set(int p, const S& x)
S get(int p)
```

## 時間計算量

- 償却 $O(\log N)$

## 区間操作

```cpp
void apply(int l, int r, const T& f)
void reverse(int l, int r)
S prod(int l, int r)
S all_prod()
```

半開区間 `[l,r)` を扱う。

## 時間計算量

- 償却 $O(\log N)$

## API 別の計算量

$N$ を操作前の列長とし、モノイド演算・作用を $O(1)$ とする。

- デフォルトコンストラクタ: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(1)$
- `LazyReversibleSplayTree(values)`: 長さ $N$ の平衡木構築 $O(N)$（固定長版はさらに固定長領域の初期化 $O(\texttt{MAX_SIZE})$）
- `size`, `empty`, `clear`, `all_prod`: $O(1)$
- `insert`, `push_front`, `push_back`, `erase`, `set`, `get`: 最悪 $O(N)$、操作列に対して償却 $O(\log(N+1))$
- `apply`, `reverse`, `prod`: 最悪 $O(N)$、操作列に対して償却 $O(\log(N+1))$

区間操作は内部の定数回の分割・併合を含む。

## 空間計算量

- オブジェクトと、テンプレートの実体ごとに共有される祖先スタック: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は確保したノード数に比例
- splay が使用する祖先スタック要素数: 最悪 $O(N)$
- `std::vector` からのコンストラクタの再帰スタック: $O(\log(N+1))$

## 注意点

- `insert(p,x)` は `0 <= p <= size()`、点操作は `0 <= p < size()`。
- `apply`, `reverse`, `prod` は `0 <= l <= r <= size()`。違反すると `std::runtime_error` を投げる。
- `insert` で要素数が `MAX_SIZE` を超える場合は `std::runtime_error` を投げる。
- `prod` は左から右の順で合成し、空区間では単位元を返す。
- `composition(f,g)` は先に `g`、後に `f` を表す必要がある。
- `clear` は確保済みのノード領域を再利用し、コピーはできないがムーブはできる。
- splay 用の祖先スタックは `static` 変数でテンプレートの実体ごとに共有されるので、同じ型のオブジェクトを複数スレッドから同時に操作してはならない。
