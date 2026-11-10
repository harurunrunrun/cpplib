---
title: Count Distinct Substrings (相異なる部分文字列数) [DISUBSTR] [number_of_substrings]
documentation_of: ../../../../src/algorithm/string/suffix/count_distinct_substrings.hpp
---

空でない相異なる連続部分列の個数を返す。

## API

~~~cpp
long long count_distinct_substrings(const std::string& sequence)
template<class Sequence>
long long count_distinct_substrings(const Sequence& sequence)
~~~

`sequence` に現れる、空でない相異なる連続部分列の個数を返す。
空列では `0` を返す。

## 時間計算量

$N$ を列の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_distinct_substrings(const std::string&)` | $O(N)$ | $O(N)$ |
| `count_distinct_substrings(const Sequence&)` | $O(N\log N)$ | $O(N)$ |

`std::string` 版はバイト単位の suffix automaton の各状態について
`length[state] - length[link[state]]` を合計する。
一般列版は suffix array と LCP 配列を使い、$N(N+1)/2$ から LCP の総和を引く。

## 注意点

- 一般列の `Sequence` は `suffix_array` と `lcp_array` が受理する型（`std::vector<T>`）でなければならない。
- `std::string` 版は各要素を `unsigned char` に変換するため、全 256 種のバイトを区別する。
- `std::string` 版で suffix automaton を構築する入力長 $N$ は
  `(std::numeric_limits<int>::max() / 2) + 1` 以下でなければならない。
  超える場合は、状態を追加する前に `std::length_error` を送出する。
- 戻り値には空の部分列を含めない。
- 戻り値型は `long long` である。
