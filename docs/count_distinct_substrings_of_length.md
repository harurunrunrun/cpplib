---
title: Count Distinct Substrings of Fixed Length (固定長の相異なる部分文字列数) [ADACLEAN]
documentation_of: ../src/algorithm/string/count_distinct_substrings_of_length.hpp
---

指定した長さの相異なる連続部分列数を返す。

# API

~~~cpp
template<class Sequence>
long long count_distinct_substrings_of_length(
    const Sequence& sequence,
    int length
)
~~~

`sequence` に現れる長さ `length` の相異なる連続部分列数を返す。
`length <= 0` または `length > sequence.size()` では `0` を返す。

## 時間計算量

$N$ を列の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_distinct_substrings_of_length(sequence,length)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `Sequence` は `suffix_array` と `lcp_array` が受理する型でなければならない。
- 戻り値型は `long long` である。
