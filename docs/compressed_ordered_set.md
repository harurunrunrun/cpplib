---
title: Compressed Ordered Set (座標圧縮順序付き集合) [ordered_set]
documentation_of: ../src/structure/tree/compressed_ordered_set.hpp
---

任意の値を座標圧縮し、重複を持たない順序付き集合として管理する。圧縮後の添字管理には `Int_Set` を使う。

## 構築

```cpp
CompressedOrderedSet<Value, MAX_UNIQUE> set(universe, initial)
```

`universe` と `initial` を合わせて昇順に圧縮し、`initial` の各値を集合へ挿入する。異なる値の総数が `MAX_UNIQUE` を超える場合は `runtime_error`。

- 時間計算量: $O((U+I)\log(U+I)+I\log MAX\_UNIQUE)$
- 空間計算量: 座標列 $O(U+I)$ と `Int_Set` が生成するnode

## 更新

```cpp
void insert(const Value& value)
void erase(const Value& value)
```

登録済みの値を挿入・削除する。重複挿入と存在しない値の削除は状態を変えない。

- 時間計算量: $O(\log U+\log MAX\_UNIQUE)$

## 順序query

```cpp
optional<Value> kth_smallest(int index)
int order_of_key(const Value& value)
optional<Value> kth_smallest_one_based(int rank)
int count_less_equal(const Value& value)
optional<Value> predecessor_or_equal(const Value& value)
optional<Value> successor_or_equal(const Value& value)
```

`kth_smallest` は0-indexed、`kth_smallest_one_based` は1-indexedで、負の添字・0以下のrank・存在しない順位では `nullopt`。`order_of_key` は `value` 未満、`count_less_equal` は `value` 以下の要素数を返す。前者・後者探索も存在しなければ `nullopt`。

- `kth_smallest`, `kth_smallest_one_based`: $O(\log U+16\log_{16} MAX\_UNIQUE)$
- その他: $O(\log U+\log_{16} MAX\_UNIQUE)$

## その他

```cpp
int size()
bool empty()
bool contains(const Value& value)
```

`size` は $O(\log_{16} MAX\_UNIQUE)$、`empty` は $O(1)$、`contains` は $O(\log U+\log_{16} MAX\_UNIQUE)$。

## 注意点

- `insert` と `erase` の値は構築時の `universe` または `initial` に登録済みでなければならず、未登録値では `runtime_error`。
- `contains`、個数query、前者・後者queryの境界値は未登録でもよい。
- 異なる値がちょうど `MAX_UNIQUE` 個の場合も利用でき、添字範囲は `[0, MAX_UNIQUE)`。
- `MAX_UNIQUE > 0`。通常操作の追加領域は木の高さに比例する。
