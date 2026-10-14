---
title: AA Tree Ordered Dictionary (AA木順序付き辞書)
documentation_of: ../../../../src/structure/bbst/map/aa_tree_map.hpp
---

AA treeで実装した順序付き連想配列。赤黒木の水平linkを右向きに限定した平衡条件を
`skew` と `split` で保つ。nodeは容量 `MAX_SIZE` の `std::array` poolに置き、
削除nodeを再利用する。部分木sizeも保持するため順位queryを行える。

```cpp
template<class Key, class T, int MAX_SIZE, class Compare = std::less<Key>>
struct AATreeMap;
```

`Compare` はstrict weak orderingを満たす必要がある。

## 構築・状態

```cpp
explicit AATreeMap(const Compare& compare = Compare())
static constexpr int capacity()
int size() const
bool empty() const
void clear()
```

空の辞書を作る。`clear` は全keyを削除する。

- 構築: 時間 $O(MAX\_SIZE)$、保存領域 $O(MAX\_SIZE)$
- `capacity`, `size`, `empty`: $O(1)$
- `clear`: $O(A)$（一度でも確保したnode数 $A$）

## `insert` / `insert_or_assign`

```cpp
bool insert(const Key& key, const T& mapped)
bool insert_or_assign(const Key& key, const T& mapped)
```

`insert` は未登録keyだけを追加する。`insert_or_assign` は既存なら値を置換する。
戻り値は新しいkeyを追加したか。満杯で新規追加すると `runtime_error`。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: 再帰stack $O(\log(N+1))$

## `erase`

```cpp
bool erase(const Key& key)
```

keyを削除する。戻り値は存在したか。解放nodeは次回の挿入で再利用する。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: 再帰stack $O(\log(N+1))$

## 検索

```cpp
bool contains(const Key& key) const
T* find(const Key& key)
const T* find(const Key& key) const
T& at(const Key& key)
const T& at(const Key& key) const
```

`find` は未登録なら `nullptr`。`at` は未登録なら `runtime_error`。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## bound query

```cpp
const value_type* lower_bound(const Key& key) const
const value_type* upper_bound(const Key& key) const
```

`lower_bound` は `key` 以上、`upper_bound` は `key` より大きい最初の要素を
`Compare` 順で返す。該当なしなら `nullptr`。返したpointerは辞書の次の変更まで
有効である。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## 順位query

```cpp
int order_of_key(const Key& key) const
const value_type& kth(int index) const
```

`order_of_key` は `key` 未満のkey数を返す。`kth` は0-indexedで `Compare` 順の
要素を返し、範囲外なら `runtime_error`。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## 注意点

keyは重複しない。objectの保存領域は常に $O(MAX\_SIZE)$。再帰の深さはAA treeの
高さ $O(\log(N+1))$。`Key` と `T` はcopy可能である必要がある。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = AATreeMap<Key, T>;
using Fixed = AATreeMap<Key, T, N>;
```

可変長版は実行時の要素数・頂点数に応じて配列を確保する。ノードを追加する構造では伸長費用が償却 $O(1)$、既存の操作計算量を維持する。
領域は使用ノード数に比例し、木の距離索引など既存の段別領域は上記と同じ漸近量。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
