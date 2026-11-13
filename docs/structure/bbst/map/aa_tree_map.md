---
title: AA Tree Ordered Dictionary (AA木順序付き辞書)
documentation_of: ../../../../src/structure/bbst/map/aa_tree_map.hpp
---

AA tree で実装した順序付き連想配列。赤黒木の水平リンクを右向きに限定した平衡条件を `skew` と `split` で保つ。ノードは配列上のプールに置き、削除したノードを再利用する。部分木の大きさも保持するので順位クエリを行える。

```cpp
template<class Key, class T, int MAX_SIZE = -1, class Compare = std::less<Key>>
struct AATreeMap;
```

`MAX_SIZE` を指定すると、容量 `MAX_SIZE` のノードプールを `std::array` で固定確保する。省略（`-1`）すると、ノードを `std::vector` に必要に応じて確保する可変長版になる（ノード追加の伸長コストは償却 $O(1)$ で、以下の計算量は変わらない）。要素型は `value_type = std::pair<Key, T>`。

`Compare` は狭義弱順序を満たす必要がある。以下、現在の要素数を $N$ とする。

## 構築・状態

```cpp
explicit AATreeMap(const Compare& compare = Compare())
static constexpr int capacity()
int size() const
bool empty() const
void clear()
```

空の辞書を作る。`clear` は全キーを削除する。

- 構築: 固定長版は時間・保存領域とも $O(\texttt{MAX_SIZE})$、可変長版は $O(1)$
- `capacity`: 固定長版は `MAX_SIZE`、可変長版は `std::numeric_limits<int>::max()` を返す
- `capacity`, `size`, `empty`: $O(1)$
- `clear`: $O(A)$（一度でも確保したノード数 $A$）

## `insert` / `insert_or_assign`

```cpp
bool insert(const Key& key, const T& mapped)
bool insert_or_assign(const Key& key, const T& mapped)
```

`insert` は未登録キーだけを追加する。`insert_or_assign` は既存なら値を置換する。戻り値は新しいキーを追加したかどうか。満杯の状態で新規追加すると `std::runtime_error` を投げる。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: 再帰スタック $O(\log(N+1))$

## `erase`

```cpp
bool erase(const Key& key)
```

キーを削除する。戻り値はキーが存在したかどうか。解放したノードは次回以降の挿入で再利用する。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: 再帰スタック $O(\log(N+1))$

## 検索

```cpp
bool contains(const Key& key) const
T* find(const Key& key)
const T* find(const Key& key) const
T& at(const Key& key)
const T& at(const Key& key) const
```

`find` は未登録なら `nullptr` を返す。`at` は未登録なら `std::runtime_error` を投げる。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## lower_bound / upper_bound

```cpp
const value_type* lower_bound(const Key& key) const
const value_type* upper_bound(const Key& key) const
```

`lower_bound` は `key` 以上、`upper_bound` は `key` より大きい最初の要素を `Compare` 順で返す。該当なしなら `nullptr`。返したポインタは辞書の次の変更まで有効である。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## 順位クエリ

```cpp
int order_of_key(const Key& key) const
const value_type& kth(int index) const
```

`order_of_key` は `key` 未満のキーの個数を返す。`kth` は `Compare` 順で 0-indexed の `index` 番目の要素を返し、範囲外なら `std::runtime_error` を投げる。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## 注意点

- キーは重複しない。
- 固定長版のオブジェクトの保存領域は常に $O(\texttt{MAX_SIZE})$、可変長版は一度でも確保したノード数に比例する。
- 再帰の深さは AA tree の高さ $O(\log(N+1))$ である。
- `Key` と `T` はコピー可能である必要がある。
