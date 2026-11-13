---
title: MEX Multiset (MEX多重集合)
documentation_of: ../../../../src/structure/ordered_set/integer/mex_multiset.hpp
---

有限の整数領域 $[0, U)$ 上の多重集合を管理し、含まれない値のうち最小のもの（MEX）や $k$ 番目のものを求める。

## コンストラクタ

```cpp
explicit MexMultiset(size_t universe)
```

領域の長さが $U$ = `universe` の空の多重集合を構築する。

## insert / erase / count

```cpp
bool insert(size_t value)
bool erase(size_t value)
size_t count(size_t value) const
```

`insert` は値 `value` を 1 個追加し、追加前に `value` が含まれていなかったかを返す。`erase` は `value` を 1 個削除し、削除によって出現数が 0 になったかを返す。含まれていない値の `erase` は何もせず `false` を返す。`count` は `value` の出現数を返す。

## mex / kth_missing

```cpp
size_t mex() const
optional<size_t> kth_missing(size_t index) const
```

`mex` は含まれない最小の値を返し、領域内の値がすべて含まれていれば $U$ を返す。`kth_missing` は領域内で含まれない値のうち 0-indexed で `index` 番目のものを返し、そのような値が足りなければ `std::nullopt` を返す。

## universe / distinct_size / empty

```cpp
size_t universe() const
size_t distinct_size() const
bool empty() const
```

それぞれ領域の長さ $U$、含まれる相異なる値の個数、空かどうかを返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(U)$ | $O(U)$ |
| `insert`, `erase`, `mex`, `kth_missing` | $O(\log U)$ | $O(1)$ |
| `count`, `universe`, `distinct_size`, `empty` | $O(1)$ | $O(1)$ |

## 注意点

`insert`, `erase`, `count` は値が $[0, U)$ にあることを要求し、範囲外なら `std::out_of_range` を送出する。`kth_missing` は領域外の整数を含まれない値として数えない。
