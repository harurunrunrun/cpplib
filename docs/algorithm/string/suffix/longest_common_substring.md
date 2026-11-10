---
title: Longest Common Substring (最長共通部分文字列) [longest_common_substring]
documentation_of: ../../../../src/algorithm/string/suffix/longest_common_substring.hpp
---

2 つの文字列の最長共通部分文字列を求める。

## API

```cpp
struct LongestCommonSubstringResult{
    int first_pos;
    int second_pos;
    int length;
};
LongestCommonSubstringResult longest_common_substring(
    const std::string& a, const std::string& b
)
```

- `first_pos`: 第 1 文字列 `a` における開始位置。
- `second_pos`: 第 2 文字列 `b` における開始位置。
- `length`: 共通部分文字列の長さ。
- `longest_common_substring(a, b)` は
  `a.substr(first_pos, length) == b.substr(second_pos, length)` を満たす最長の結果を返す。
  共通する空でない文字列がなければ `{0, 0, 0}` を返す。同じ最大長の候補が複数ある場合、どの位置を返すかは規定しない。

## API 別の時間計算量・空間計算量

$S = \lvert a\rvert + \lvert b\rvert$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `longest_common_substring(a, b)` | $O(S)$ | $O(\lvert a\rvert)$ |
| `LongestCommonSubstringResult` の各メンバの参照 | $O(1)$ | - |

第 1 文字列からバイト単位の suffix automaton を構築し、第 2 文字列を suffix link で後退しながら 1 回走査する。
遷移と suffix link の処理回数は償却で線形である。

## 注意点

- 文字は `unsigned char` として扱うため、全 256 種のバイトを利用できる。
- 返す位置は 0-indexed である。
- 第 1 文字列 `a` の長さは `(std::numeric_limits<int>::max() / 2) + 1` 以下、
  第 2 文字列 `b` の長さは `std::numeric_limits<int>::max()` 以下でなければならない。
  違反時は `std::length_error` を送出する。`a` の違反は状態を追加する前に検出する。
