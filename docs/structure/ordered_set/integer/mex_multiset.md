---
title: MEX Multiset (MEX多重集合)
documentation_of: ../../../../src/structure/ordered_set/integer/mex_multiset.hpp
---

有限整数領域 `[0,U)` の重複付き集合を管理し、最小または $k$ 番目の欠損値を求める。

## constructor

```cpp
explicit MexMultiset(size_t universe)
```

領域長 `U=universe` の空集合を作る。

## insert / erase / count

```cpp
bool insert(size_t value)
bool erase(size_t value)
size_t count(size_t value) const
```

`insert` は挿入前に未出現だったか、`erase` は削除によって出現数が 0 になったかを返す。存在しない値の削除は `false` を返す。

## mex / kth_missing

```cpp
size_t mex() const
optional<size_t> kth_missing(size_t index) const
```

`mex` は最小欠損値を返し、全値が存在すれば `U` を返す。`kth_missing` は領域内の 0 始まり `index` 番目を返し、個数不足なら `nullopt` を返す。

## universe / distinct_size / empty

```cpp
size_t universe() const
size_t distinct_size() const
bool empty() const
```

領域長、異なる存在値数、空かを返す。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor | $O(U)$ | $O(U)$ |
| `insert`, `erase`, `mex`, `kth_missing` | $O(\log U)$ | $O(1)$ |
| `count`, `universe`, `distinct_size`, `empty` | $O(1)$ | $O(1)$ |

## 注意点

値 API は `[0,U)` を要求し、範囲外なら `out_of_range` を送出する。`kth_missing` は領域外の整数を欠損値として数えない。
