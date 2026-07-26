---
title: Count Distinct Substrings (相異なる部分文字列数) [DISUBSTR] [number_of_substrings]
documentation_of: ../../../../src/algorithm/string/suffix/count_distinct_substrings.hpp
---

空でない相異なる連続部分列の個数を返す。

## API

~~~cpp
long long count_distinct_substrings(const string& sequence)
template<class Sequence>
long long count_distinct_substrings(const Sequence& sequence)
~~~

`sequence` に現れる、空でない相異なる連続部分列の個数を返す。
空列では `0` を返す。

## 時間計算量

$N$ を列の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_distinct_substrings(const string&)` | $O(N)$ | $O(N)$ |
| `count_distinct_substrings(const Sequence&)` | $O(N\log N)$ | $O(N)$ |

`string` 版はbyte suffix automatonの各状態について
`length[state] - length[link[state]]` を合計する。一般列版はsuffix arrayとLCPを使う。

## 注意点

- 一般列の `Sequence` は `suffix_array` と `lcp_array` が受理する型でなければならない。
- `string` 版は各要素を `unsigned char` に変換するため、全256 byteを区別する。
- `string` 版でsuffix automatonを構築する入力長 `N` は
  `(numeric_limits<int>::max() / 2) + 1` 以下でなければならない。
  超える場合は入力文字に対応するstate追加前に `std::length_error` を送出する。
- 戻り値には空部分列を含めない。
- 戻り値型は `long long` である。
