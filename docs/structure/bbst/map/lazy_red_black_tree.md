---
title: Lazy Red Black Tree (遅延赤黒木)
documentation_of: ../../../../src/structure/bbst/map/lazy_red_black_tree.hpp
---

赤黒木（左傾赤黒木）によるキー付きの列。キーの半開区間に対して、値への作用と値の積を扱う。

## テンプレート引数

```cpp
LazyRedBlackTree<Key, MonoidActLen, MAX_SIZE = -1, Compare = std::less<Key>>
```

- `Key`: キーの型
- `MonoidActLen`: 作用付きモノイド（区間長付き）のオブジェクト
- `MAX_SIZE`: 同時に保持する要素数の上限。`-1`（省略時）なら上限なしの可変長版
- `Compare`: 比較関数

`MAX_SIZE` を指定するとノード領域を固定確保し、省略するとノードを `std::vector` に必要に応じて確保する（伸長コストは償却 $O(1)$）。

`MonoidActLen` は型 `S`, `T` と次の関数を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

## コンストラクタ・状態

```cpp
explicit LazyRedBlackTree(const Compare& compare = Compare())
int size() const
bool empty() const
void clear()
```

空の木を構築する。`clear` は全要素を削除し、ノード領域を再利用する。コピーはできないが、ムーブはできる。

## insert / erase / contains

```cpp
bool insert(const Key& key, const S& value)
bool erase(const Key& key)
bool contains(const Key& key) const
```

`insert` は同じキーが存在する場合は何もせず `false` を返す。`erase` はキーが存在しない場合 `false` を返す。

## get / set

```cpp
std::optional<S> get(const Key& key)
bool set(const Key& key, const S& value)
```

`get` はキーが存在しない場合 `std::nullopt` を返す。`set` はキーが存在しない場合 `false` を返す。

## 区間操作

```cpp
void apply(const Key& l, const Key& r, const T& f)
S prod(const Key& l, const Key& r)
void all_apply(const T& f)
S all_prod() const
```

キーの半開区間 $[l,r)$ に属する要素の値に `f` を作用させる／値の積を返す。`all_apply`, `all_prod` は全体に対して行う。

## 順序

```cpp
int order_of_key(const Key& key) const
std::optional<Key> kth_key(int k) const
```

`order_of_key` は `key` 未満のキーの個数を返す。`kth_key` は 0-indexed で `k` 番目のキーを返し、範囲外なら `std::nullopt` を返す。

## 時間計算量

$N$ を現在の要素数とし、比較・モノイド演算・作用を $O(1)$ とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE})$（固定長領域を確保・初期化する）、可変長版は $O(1)$
- `size`, `empty`, `clear`, `all_apply`, `all_prod`: $O(1)$
- `contains`, `insert`, `erase`, `get`, `set`: 最悪 $O(\log(N+1))$
- `apply`, `prod`: 最悪 $O(\log(N+1))$
- `order_of_key`, `kth_key`: 最悪 $O(\log(N+1))$

キー区間の操作は境界に至る経路だけを展開し、区間に完全に含まれる部分木は遅延タグまたは集約値でまとめて処理する。

## 空間計算量

- オブジェクト全体: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は一度でも確保したノード数に比例
- `set` の経路用の一時配列: 木の高さの上限に合わせた固定長（128 要素）をスタック上に置く。$O(1)$
- `insert`, `erase`, `apply`, `prod` の再帰スタック: $O(\log(N+1))$
- その他の操作の追加領域: $O(1)$

## 注意点

- `Compare` は狭義弱順序でなければならない。同値なキーは 1 個だけ保持する。
- 新規の `insert` で要素数が `MAX_SIZE` を超える場合は `std::runtime_error` を投げる。
- `apply(l, r, f)` と `prod(l, r)` は `Compare(l, r)` が偽（区間が空または逆転）なら、何もしない／単位元を返す。例外は投げない。
