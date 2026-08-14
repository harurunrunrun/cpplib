---
title: Persistent MEX Multiset (永続MEX多重集合)
documentation_of: ../../../../src/structure/ordered_set/integer/persistent_mex_multiset.hpp
---

有限整数領域の多重集合を path copying で完全永続化し、任意 version の MEX を求める。

## constructor

```cpp
explicit PersistentMexMultiset(size_t universe)
```

空集合を version 0 として構築する。

## insert / erase

```cpp
size_t insert(size_t version, size_t value)
size_t erase(size_t version, size_t value)
```

指定 version から更新した新 version を作り、その番号を返す。存在しない値の `erase` も同内容の新 version を作る。

## count / distinct_size / mex

```cpp
size_t count(size_t version, size_t value) const
size_t distinct_size(size_t version) const
size_t mex(size_t version) const
```

指定 version の出現数、異なる値数、MEX を返す。領域が完全なら MEX は `universe` である。

## universe / version_count / node_count

領域長、version 数、確保 node 数を $O(1)$ で返す。

## 時間計算量

$U$ を領域長とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| constructor | $O(U)$ | $O(U)$ |
| `insert`, 存在値の `erase` | $O(\log U)$ | $O(\log U)$ |
| 不存在値の `erase` | $O(\log U)$ | $O(1)$ |
| `count`, `mex` | $O(\log U)$ | $O(1)$ |
| `distinct_size`, getter | $O(1)$ | $O(1)$ |

## 注意点

値は `[0,U)`、version は `[0,version_count())` を要求し、違反時は `out_of_range` を送出する。既存 version は更新されない。
