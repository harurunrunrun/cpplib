---
title: Longest Common Substring Length (最長共通部分文字列長) [ADAPHOTO]
documentation_of: ../src/algorithm/string/longest_common_substring_length.hpp
---

2つのbyte列に共通する連続部分列の最大長を求める。

# API

```cpp
int longest_common_substring_length(string_view first, string_view second)
```

`first` と `second` の両方に現れる連続部分列の最大長を返す。
空文字列を渡した場合は0を返す。文字列はASCIIに限らず、`'\0'` を含む任意の
byte列でよい。

## 時間計算量

$N=|first|$, $M=|second|$ とする。byte alphabetの大きさは高々256なので、
時間計算量は $O(N+M)$、空間計算量は $O(\min(N,M))$。

## 注意点

内部では短い側からsuffix automatonを構築する。短い側の長さが
`(INT_MAX - 1) / 2` を超える場合は `std::length_error` を送出する。
同じ長さを持つ部分文字列の位置や内容は返さない。
