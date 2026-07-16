---
title: Ordered Associative Aggregate (順序付き連想集約)
documentation_of: ../src/structure/bbst/aa_tree_monoid_map.hpp
---

AA tree で実装した、重複しない key とモノイド値の順序付き連想配列。
各 node に部分木の要素数と key 順の積を保持し、辞書操作、順位操作、key 区間の積を
worst-case 対数時間で処理する。加法モノイドを指定すれば Ordered Associative Array
の区間和になる。積の順序を保存するため、モノイドは可換でなくてもよい。

```cpp
template<
    class Key,
    auto Monoid,
    int MAX_SIZE,
    class Compare = std::less<Key>
>
struct AATreeMonoidMap;
```

`Monoid` は `using S`、`S op(S,S)`、`S e()` を持つモノイド object である。
`mapped_type` は `Monoid::S`、`value_type` は `pair<Key,mapped_type>` である。
各 API の時間計算量では、$N$ を現在の要素数、モノイド演算と key 比較を $O(1)$ とする。

## 構築と状態

```cpp
explicit AATreeMonoidMap(const Compare& compare = Compare())
static constexpr int capacity()
int size() const
bool empty() const
void clear()
```

空の連想配列を構築する。`capacity` は同時に保持できる最大要素数、`size` は現在の
要素数を返す。`clear` は全要素を削除し、node pool を先頭から再利用できる状態に戻す。

- 構築: 時間・保存領域 $O(MAX\_SIZE)$
- `capacity`, `size`, `empty`: 時間 $O(1)$、追加領域 $O(1)$
- `clear`: 時間 $O(A)$、追加領域 $O(1)$。$A$ は直前の `clear` 以降に確保した node 数

## 挿入と更新

```cpp
bool insert(const Key& key, const mapped_type& value)
bool insert_or_assign(const Key& key, const mapped_type& value)
bool set(const Key& key, const mapped_type& value)
```

`insert` は key が未登録のときだけ追加する。`insert_or_assign` は未登録なら追加し、
登録済みなら値を置換する。この2関数の戻り値は新しい key を追加したかを表す。
`set` は登録済みの値だけを置換し、key が存在したかを返す。

- 時間: worst-case $O(\log(N+1))$
- 追加領域: 再帰 stack $O(\log(N+1))$

新規挿入によって `MAX_SIZE` を超える場合は `runtime_error` を送出する。

## 削除

```cpp
bool erase(const Key& key)
```

指定 key を削除し、存在したかを返す。解放した node は後の挿入で再利用する。

- 時間: worst-case $O(\log(N+1))$
- 追加領域: 再帰 stack $O(\log(N+1))$

## key 検索

```cpp
bool contains(const Key& key) const
const mapped_type* find(const Key& key) const
const mapped_type& at(const Key& key) const
```

`contains` は key の有無を返す。`find` は未登録なら `nullptr`、`at` は未登録なら
`runtime_error` を送出する。返される値は集約を壊さないよう const であり、変更には
`set` または `insert_or_assign` を使う。

- 時間: worst-case $O(\log(N+1))$
- 追加領域: $O(1)$

## bound

```cpp
const value_type* lower_bound(const Key& key) const
const value_type* upper_bound(const Key& key) const
```

`lower_bound` は `Compare` 順で `key` 以上、`upper_bound` は `key` より後にある
最初の要素を返す。該当要素がなければ `nullptr`。pointer は次に連想配列を変更する
まで有効である。

- 時間: worst-case $O(\log(N+1))$
- 追加領域: $O(1)$

## 順位

```cpp
int order_of_key(const Key& key) const
int order_of_key_inclusive(const Key& key) const
const value_type& kth(int index) const
```

`order_of_key` は `Compare` 順で `key` より前にある要素数、
`order_of_key_inclusive` は `key` と同値な要素も含めた個数を返す。`kth` は
0-indexed の要素を返し、`index` が範囲外なら `runtime_error` を送出する。

- 時間: worst-case $O(\log(N+1))$
- 追加領域: $O(1)$

## key 区間積

```cpp
mapped_type prod(const Key& lower, const Key& upper) const
mapped_type prefix_prod(const Key& upper) const
mapped_type all_prod() const
```

`prod` は `Compare` 順の半開区間 $[lower,upper)$ に属する値を key 順に積む。
`prefix_prod` は `upper` より前、`all_prod` は全要素の値を key 順に積む。
空集合と、`lower` が `upper` より前でない区間には単位元を返す。

- `prod`, `prefix_prod`: 時間 worst-case $O(\log(N+1))$、追加領域 $O(1)$
- `all_prod`: 時間 $O(1)$、追加領域 $O(1)$

## 注意点

- `Compare` は strict weak ordering を満たす必要がある。同値な key は1個だけ保持する。
- `Monoid::op` は結合的で、`Monoid::e()` は左右の単位元でなければならない。
- `Key` と `mapped_type` は copy 可能である必要がある。
- object の保存領域は要素数によらず $O(MAX_SIZE)$ である。
