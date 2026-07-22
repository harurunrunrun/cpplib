---
title: Longest Common Substring (最長共通部分文字列) [longest_common_substring]
documentation_of: ../src/algorithm/string/longest_common_substring.hpp
---

2 つの文字列の最長共通部分文字列を求める。

## 構造体

- `LongestCommonSubstringResult`
    - `first_pos`
    - `second_pos`
    - `length`

## 関数

- `longest_common_substring(a, b)`

## 計算量の概要

- $O(n+m)$

## API別の時間計算量・空間計算量

$S=|a|+|b|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `longest_common_substring(a,b)` | $O(S)$ | $O(\lvert a\rvert)$ |
| `LongestCommonSubstringResult` の各field参照 | $O(1)$ | - |

第1文字列からbyte suffix automatonを構築し、第2文字列をsuffix linkで後退しながら
1回走査する。各遷移・suffix link処理の償却回数は線形である。

## API詳細

- `first_pos`: 第1文字列 `a` における開始位置。
- `second_pos`: 第2文字列 `b` における開始位置。
- `length`: 共通部分文字列の長さ。
- `longest_common_substring(a,b)` は
  `a.substr(first_pos,length) == b.substr(second_pos,length)` を満たす最長結果を返す。
  共通する非空文字列がなければ `{0,0,0}`。同じ最大長の候補が複数ある場合の
  位置は規定しない。

## 注意点

- 文字は `unsigned char` として扱うため、全256 byteを利用できる。
- 返すindexと区間は0-indexedである。
- 入力長と位置は `int` で表現できなければならない。
