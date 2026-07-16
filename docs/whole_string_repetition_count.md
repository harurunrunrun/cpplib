---
title: Whole String Repetition Count (文字列全体の反復回数) [FINDSR]
documentation_of: ../src/algorithm/string/whole_string_repetition_count.hpp
---

列全体を同じ非空ブロックの反復として表すときの最大反復回数を返す。

## API

~~~cpp
template<class Sequence>
int whole_string_repetition_count(const Sequence& sequence)
~~~

`sequence = block + block + ... + block` と表せる最大のブロック数を返す。
空列では `0`、空でない非周期列では `1` を返す。

## 時間計算量

$N$ を列の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `whole_string_repetition_count(sequence)` | $O(N)$ | $O(N)$ |

## 注意点

- `Sequence` は `prefix_function` が受理する型でなければならない。
- 要素の等値比較1回を $O(1)$ とする。
