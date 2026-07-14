---
title: Count Distinct Substrings (相異なる部分文字列数) [DISUBSTR] [number_of_substrings]
documentation_of: ../src/algorithm/string/count_distinct_substrings.hpp
---

空でない相異なる連続部分列の個数を返す。

# API

~~~cpp
template<class Sequence>
long long count_distinct_substrings(const Sequence& sequence)
~~~

`sequence` に現れる、空でない相異なる連続部分列の個数を返す。
空列では `0` を返す。

## 時間計算量

$N$ を列の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_distinct_substrings(sequence)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `Sequence` は `suffix_array` と `lcp_array` が受理する型でなければならない。
- 戻り値には空部分列を含めない。
- 戻り値型は `long long` である。
