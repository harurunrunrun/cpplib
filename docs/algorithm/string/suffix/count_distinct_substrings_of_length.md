---
title: Count Distinct Substrings of Fixed Length (固定長の相異なる部分文字列数) [ADACLEAN]
documentation_of: ../../../../src/algorithm/string/suffix/count_distinct_substrings_of_length.hpp
---

指定した長さの相異なる連続部分列の個数を返す。

## API

~~~cpp
long long count_distinct_substrings_of_length(
    const std::string& sequence,
    int length
)
template<class Sequence>
long long count_distinct_substrings_of_length(
    const Sequence& sequence,
    int length
)
~~~

`sequence` に現れる長さ `length` の相異なる連続部分列の個数を返す。
`length <= 0` または `length > sequence.size()` の場合は `0` を返す。

## 時間計算量

$N$ を列の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_distinct_substrings_of_length(const std::string&, length)` | $O(N)$ | $O(N)$ |
| `count_distinct_substrings_of_length(const Sequence&, length)` | $O(N\log N)$ | $O(N)$ |

`std::string` 版では、suffix automaton の状態 `v` が長さ $L$ の部分文字列を表す条件
`length[link[v]] < L <= length[v]` を満たす状態を全状態について数える。
一般列版は suffix array と LCP 配列を使う。

## 注意点

- 一般列の `Sequence` は `suffix_array` と `lcp_array` が受理する型（`std::vector<T>`）でなければならない。
- `std::string` 版は各要素を `unsigned char` に変換するため、全 256 種のバイトを区別する。
- `1 <= length <= sequence.size()` の場合、`std::string` 版の入力長 $N$ は
  `(std::numeric_limits<int>::max() / 2) + 1` 以下でなければならない。
  超える場合は、状態を追加する前に `std::length_error` を送出する。
- 戻り値型は `long long` である。
