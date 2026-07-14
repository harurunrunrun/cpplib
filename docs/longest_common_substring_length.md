---
title: Longest Common Substring Length (最長共通部分文字列長) [ADAPHOTO] [LCS]
documentation_of: ../src/algorithm/string/longest_common_substring_length.hpp
---

2つのbyte列に共通する連続部分列の最大長を求める。

## API

```cpp
int longest_common_substring_length(string_view first, string_view second)
```

## 引数・戻り値

- `first`, `second`: 比較するbyte列。ASCIIに限らず `'\0'` を含められる。
- 戻り値: 両方に現れる連続部分列の最大長。いずれかが空なら `0` を返す。

## API別の時間計算量・空間計算量

$N=|first|$, $M=|second|$ とし、byte alphabetの大きさを定数とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `longest_common_substring_length(first,second)` | $O(N+M)$ | $O(\min(N,M))$ |

## 注意点

- 短い側の長さが `(INT_MAX - 1) / 2` を超える場合は
  `length_error` を送出する。
- 同じ長さを持つ部分文字列の位置や内容は返さない。
- `string_view` が参照するbyte列は呼び出し中有効でなければならない。
