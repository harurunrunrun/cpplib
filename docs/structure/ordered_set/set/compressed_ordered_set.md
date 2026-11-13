---
title: Compressed Ordered Set (座標圧縮順序付き集合) [ordered_set]
documentation_of: ../../../../src/structure/ordered_set/set/compressed_ordered_set.hpp
---

任意の値を座標圧縮し、重複を持たない順序付き集合として管理する。圧縮後の添字の管理には `Int_Set` を使う。

## 構築

```cpp
template<class Value, int MAX_UNIQUE = -1>
class CompressedOrderedSet;

explicit CompressedOrderedSet(std::vector<Value> universe, const std::vector<Value>& initial = {})
```

`universe` と `initial` を合わせて昇順に座標圧縮し、`initial` の各値を集合へ挿入する。相異なる値の総数が `MAX_UNIQUE`（省略時は `INT_MAX`）を超える場合は `std::runtime_error` を送出する。

$U$ を `universe` の長さ、$I$ を `initial` の長さ、$M$ を `MAX_UNIQUE`（省略時は `INT_MAX`）とする。

- 時間計算量: $O((U+I)\log(U+I)+I\log M)$
- 空間計算量: 座標列の $O(U+I)$ と、`Int_Set` が生成するノード

## 更新

```cpp
void insert(const Value& value)
void erase(const Value& value)
```

登録済みの値を挿入・削除する。すでに含まれる値の挿入と、含まれない値の削除は状態を変えない。

- 時間計算量: $O(\log U+\log M)$

## 順序クエリ

```cpp
optional<Value> kth_smallest(int index)
int order_of_key(const Value& value)
optional<Value> kth_smallest_one_based(int rank)
int count_less_equal(const Value& value)
optional<Value> predecessor_or_equal(const Value& value)
optional<Value> successor_or_equal(const Value& value)
```

- `kth_smallest` は 0-indexed、`kth_smallest_one_based` は 1-indexed で `index` / `rank` 番目に小さい要素を返す。負の `index`、0 以下の `rank`、要素数以上の順位では `std::nullopt` を返す。
- `order_of_key` は `value` 未満の要素数、`count_less_equal` は `value` 以下の要素数を返す。
- `predecessor_or_equal` は `value` 以下の最大の要素、`successor_or_equal` は `value` 以上の最小の要素を返し、存在しなければ `std::nullopt` を返す。

計算量:

- `kth_smallest`, `kth_smallest_one_based`: $O(\log U+16\log_{16} M)$
- その他: $O(\log U+\log_{16} M)$

## その他

```cpp
int size()
bool empty()
bool contains(const Value& value)
```

`size` は $O(\log_{16} M)$、`empty` は $O(1)$、`contains` は $O(\log U+\log_{16} M)$ の時間計算量である。

## 注意点

- `insert` と `erase` に渡す値は構築時の `universe` または `initial` に登録済みでなければならず、未登録の値では `std::runtime_error` を送出する。
- `contains`、個数クエリ、前者・後者クエリに渡す境界値は未登録でもよい。
- 相異なる値がちょうど `MAX_UNIQUE` 個の場合も利用でき、内部の添字範囲は `[0, MAX_UNIQUE)` である。
- `MAX_UNIQUE` は正、または省略（`-1`）であることを `static_assert` で要求する。通常の操作の追加領域は木の高さに比例する。
- `MAX_UNIQUE` を省略した場合（例: `CompressedOrderedSet<long long>`）、内部の `Int_Set` の座標上限は `INT_MAX` となるが、全座標分の配列は確保せず、必要なノードだけを動的に作る。座標列は実行時の要素数に合わせた `std::vector` に保持する。
