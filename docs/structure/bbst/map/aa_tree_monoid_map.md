---
title: Ordered Associative Aggregate (順序付き連想集約)
documentation_of: ../../../../src/structure/bbst/map/aa_tree_monoid_map.hpp
---

AA tree で実装した、重複しないキーとモノイド値の順序付き連想配列。各ノードに部分木の要素数とキー順の積を保持し、辞書操作、順位操作、キー区間の積を最悪対数時間で処理する。加法モノイドを指定すれば順序付き連想配列の区間和になる。積の順序を保存するため、モノイドは可換でなくてもよい。

```cpp
template<
    class Key,
    auto Monoid,
    int MAX_SIZE = -1,
    class Compare = std::less<Key>
>
struct AATreeMonoidMap;
```

`MAX_SIZE` を指定すると容量 `MAX_SIZE` のノードプールを固定確保し、省略（`-1`）するとノードを `std::vector` に必要に応じて確保する可変長版になる（伸長コストは償却 $O(1)$ で、以下の計算量は変わらない）。

`Monoid` は `using S`、`S op(S,S)`、`S e()` を持つモノイドのオブジェクトである。`mapped_type` は `Monoid::S`、`value_type` は `std::pair<Key, mapped_type>` である。各 API の時間計算量では、$N$ を現在の要素数、モノイド演算とキー比較を $O(1)$ とする。

## 構築と状態

```cpp
explicit AATreeMonoidMap(const Compare& compare = Compare())
static constexpr int capacity()
int size() const
bool empty() const
void clear()
```

空の連想配列を構築する。`capacity` は同時に保持できる最大要素数、`size` は現在の要素数を返す。`clear` は全要素を削除し、ノードプールを先頭から再利用できる状態に戻す。

- 構築: 固定長版は時間・保存領域 $O(\texttt{MAX_SIZE})$、可変長版は $O(1)$
- `capacity`, `size`, `empty`: 時間 $O(1)$、追加領域 $O(1)$
- `clear`: 時間 $O(A)$、追加領域 $O(1)$。$A$ は直前の `clear` 以降に確保したノード数

## 挿入と更新

```cpp
bool insert(const Key& key, const mapped_type& value)
bool insert_or_assign(const Key& key, const mapped_type& value)
bool set(const Key& key, const mapped_type& value)
```

`insert` はキーが未登録のときだけ追加する。`insert_or_assign` は未登録なら追加し、登録済みなら値を置換する。この 2 関数の戻り値は新しいキーを追加したかを表す。`set` は登録済みの値だけを置換し、キーが存在したかを返す。

- 時間: 最悪 $O(\log(N+1))$
- 追加領域: 再帰スタック $O(\log(N+1))$

新規挿入によって `MAX_SIZE` を超える場合は `std::runtime_error` を投げる。

## 削除

```cpp
bool erase(const Key& key)
```

指定キーを削除し、存在したかを返す。解放したノードは後の挿入で再利用する。

- 時間: 最悪 $O(\log(N+1))$
- 追加領域: 再帰スタック $O(\log(N+1))$

## キー検索

```cpp
bool contains(const Key& key) const
const mapped_type* find(const Key& key) const
const mapped_type& at(const Key& key) const
```

`contains` はキーの有無を返す。`find` は未登録なら `nullptr`、`at` は未登録なら `std::runtime_error` を投げる。返される値は集約を壊さないよう const であり、変更には `set` または `insert_or_assign` を使う。

- 時間: 最悪 $O(\log(N+1))$
- 追加領域: $O(1)$

## bound

```cpp
const value_type* lower_bound(const Key& key) const
const value_type* upper_bound(const Key& key) const
```

`lower_bound` は `Compare` 順で `key` 以上、`upper_bound` は `key` より後にある最初の要素を返す。該当要素がなければ `nullptr`。ポインタは次に連想配列を変更するまで有効である。

- 時間: 最悪 $O(\log(N+1))$
- 追加領域: $O(1)$

## 順位

```cpp
int order_of_key(const Key& key) const
int order_of_key_inclusive(const Key& key) const
const value_type& kth(int index) const
```

`order_of_key` は `Compare` 順で `key` より前にある要素数、`order_of_key_inclusive` は `key` と同値な要素も含めた個数を返す。`kth` は 0-indexed の要素を返し、`index` が範囲外なら `std::runtime_error` を投げる。

- 時間: 最悪 $O(\log(N+1))$
- 追加領域: $O(1)$

## キー区間積

```cpp
mapped_type prod(const Key& lower, const Key& upper) const
mapped_type prefix_prod(const Key& upper) const
mapped_type all_prod() const
```

`prod` は `Compare` 順の半開区間 $[lower,upper)$ に属する値をキー順に積む。`prefix_prod` は `upper` より前、`all_prod` は全要素の値をキー順に積む。空集合と、`lower` が `upper` より前でない区間には単位元を返す。

- `prod`, `prefix_prod`: 時間最悪 $O(\log(N+1))$、追加領域 $O(1)$
- `all_prod`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- `Compare` は狭義弱順序を満たす必要がある。同値なキーは 1 個だけ保持する。
- `Monoid::op` は結合的で、`Monoid::e()` は左右の単位元でなければならない。
- `Key` と `mapped_type` はコピー可能である必要がある。
- 固定長版のオブジェクトの保存領域は要素数によらず $O(\texttt{MAX_SIZE})$ である。可変長版は一度でも確保したノード数に比例する。
