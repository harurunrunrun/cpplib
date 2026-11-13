---
title: Longest Shared Prefix Lengths (各文字列の最長共有接頭辞長) [ABC287_E]
documentation_of: ../../../../src/algorithm/string/suffix/longest_shared_prefix_lengths.hpp
---

各文字列について、他の入力文字列と共有できる接頭辞の最大長を求める。

## API

```cpp
std::vector<std::size_t> longest_shared_prefix_lengths(const std::vector<std::string>& strings)
```

戻り値の `i` 番目は、`j != i` を満たすすべての `j` に対する `strings[i]` と `strings[j]` の LCP 長の最大値である。
比較相手が存在しない場合は `0` になる。
同じ内容の文字列が異なる添字にあれば、互いを比較相手にできる。

添字を辞書順にソートし、隣接する文字列の LCP だけを計算する。

## 時間計算量

文字列数を $N$、全文字数を $S$ とすると、ソート中の文字列比較を含めて $O(S\log(N+1))$ である。
隣接 LCP の計算は $O(S)$ である。

## 空間計算量

戻り値とソート用の添字を含めて $O(N)$ である。文字列本体は複製しない。

## 注意点

- 長さはバイト数であり、UTF-8 のコードポイント数ではない。
- 入力の順序は変更しない。
