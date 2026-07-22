---
title: Longest Shared Prefix Lengths (各文字列の最長共有接頭辞長) [ABC287_E]
documentation_of: ../src/algorithm/string/suffix/longest_shared_prefix_lengths.hpp
---

各文字列について、別の入力文字列と共有できる接頭辞の最大長を求める。

## `longest_shared_prefix_lengths`

```cpp
vector<size_t> longest_shared_prefix_lengths(const vector<string>& strings)
```

返り値の `i` 番目は、`j != i` を満たす全ての `j` に対する
`strings[i]` と `strings[j]` のLCP長の最大値である。比較相手が存在しない場合は
`0` を返す。同じ内容の文字列が異なる添字にあれば、互いを比較相手にできる。

## 時間計算量

文字列数を $N$、全文字数を $S$ とすると、文字列比較を含めて
$O(S\log(N+1))$、隣接LCP計算は $O(S)$。

## 空間計算量

返り値と整列用添字を含めて $O(N)$。文字列本体は複製しない。

## 注意点

- 長さはbyte数であり、UTF-8のコードポイント数ではない。
- 入力順は変更しない。
