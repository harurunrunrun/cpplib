---
title: Lazy Weighted Balanced Tree (遅延重み平衡木)
documentation_of: ../../../../src/structure/bbst/sequence/lazy_weighted_balanced_tree.hpp
---

重み平衡木による列。区間反転、区間作用、区間積を行う。

## テンプレート引数

```cpp
LazyWeightedBalancedTree<MonoidActLen, MAX_SIZE = -1>
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
LazyWeightedBalancedTree()
LazyWeightedBalancedTree(const std::vector<S>& values)
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
- `LazyWeightedBalancedTree(values)`: 長さ $N$ の平衡木構築 $O(N)$（固定長版はさらに固定長領域の初期化 $O(\texttt{MAX_SIZE})$）
- `size`, `empty`, `clear`, `all_prod`: $O(1)$
- `insert`, `push_front`, `push_back`, `erase`, `set`, `get`: 再平衡化を含めて償却 $O(\log(N+1))$
- `apply`, `reverse`, `prod`: 内部の分割・併合と再平衡化を含めて償却 $O(\log(N+1))$

乱数には依存しない。

## 空間計算量

- オブジェクト全体: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は確保したノード数に比例
- 各操作の再帰スタック: $O(\log(N+1))$

## 注意点

- insert 位置は `[0,size()]`、点添字は `[0,size())`、半開区間は `0 <= l <= r <= size()`。
- 範囲違反、または要素数が `MAX_SIZE` を超える `insert` は `std::runtime_error` を投げる。
- `prod` は左から右の順で合成し、空区間では単位元を返す。
- `composition(f,g)` は先に `g`、後に `f`。
- `clear` は確保済みのノード領域を再利用し、コピーはできないがムーブはできる。
