---
title: Persistent MEX Multiset (永続MEX多重集合)
documentation_of: ../../../../src/structure/ordered_set/integer/persistent_mex_multiset.hpp
---

有限の整数領域 $[0, U)$ 上の多重集合を経路コピーで完全永続化し、任意のバージョンの MEX を求める。

## コンストラクタ

```cpp
explicit PersistentMexMultiset(size_t universe)
```

領域の長さを $U$ = `universe` とし、空集合をバージョン 0 として構築する。

## insert / erase

```cpp
size_t insert(size_t version, size_t value)
size_t erase(size_t version, size_t value)
```

バージョン `version` に値 `value` を 1 個追加（削除）した新しいバージョンを作り、その番号を返す。含まれていない値の `erase` も、元と同じ内容の新しいバージョンを作る。

## count / distinct_size / mex

```cpp
size_t count(size_t version, size_t value) const
size_t distinct_size(size_t version) const
size_t mex(size_t version) const
```

バージョン `version` における `value` の出現数、含まれる相異なる値の個数、MEX を返す。領域内の値がすべて含まれていれば `mex` は $U$ を返す。

## universe / version_count / node_count

```cpp
size_t universe() const
size_t version_count() const
size_t node_count() const
```

領域の長さ、バージョン数、確保済みのノード数を $O(1)$ で返す。

## 時間計算量

$U$ を領域の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(U)$ | $O(U)$ |
| `insert`, 含まれる値の `erase` | $O(\log U)$ | $O(\log U)$ |
| 含まれない値の `erase` | $O(\log U)$ | $O(1)$ |
| `count`, `mex` | $O(\log U)$ | $O(1)$ |
| `distinct_size`, getter | $O(1)$ | $O(1)$ |

## 注意点

値は $[0, U)$、バージョンは `[0, version_count())` にあることを要求し、範囲外なら `std::out_of_range` を送出する。既存のバージョンの内容は変更されない。
