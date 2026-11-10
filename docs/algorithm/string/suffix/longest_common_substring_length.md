---
title: Longest Common Substring Length (最長共通部分文字列長) [ADAPHOTO] [LCS]
documentation_of: ../../../../src/algorithm/string/suffix/longest_common_substring_length.hpp
---

2 つのバイト列に共通する連続部分列の最大長を求める。

## API

```cpp
int longest_common_substring_length(std::string_view first, std::string_view second)
```

## 引数・戻り値

- `first`, `second`: 比較するバイト列。ASCII に限らず `'\0'` も含められる。
- 戻り値: 両方に現れる連続部分列の最大長。いずれかが空なら `0` を返す。

短い側の文字列から suffix automaton を構築し、長い側を走査する。
短い側に現れる文字の種類が $4$ 以下または $8$ 以下なら遷移を配列で持つ密な実装、それ以外では遷移を可変長の列で持つ疎な実装を使う。

## API 別の時間計算量・空間計算量

$N = \lvert \mathtt{first}\rvert$, $M = \lvert \mathtt{second}\rvert$ とし、バイトのアルファベットの大きさを定数とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `longest_common_substring_length(first, second)` | $O(N+M)$ | $O(\min(N,M))$ |

## 注意点

- 短い側の長さが `(INT_MAX - 1) / 2` を超える場合は `std::length_error` を送出する。
- 最大長を達成する部分文字列の位置や内容は返さない。
- `std::string_view` が参照するバイト列は、呼び出し中は有効でなければならない。
