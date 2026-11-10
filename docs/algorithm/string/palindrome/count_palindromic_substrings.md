---
title: Count Palindromic Substrings (回文部分文字列の個数) [NUMOFPAL]
documentation_of: ../../../../src/algorithm/string/palindrome/count_palindromic_substrings.hpp
---

文字列に含まれる空でない回文部分文字列を、出現位置ごとに数える。Manacher 法の半径の総和として求める。

## API

```cpp
long long count_palindromic_substrings(const string& text)
```

## 引数・戻り値

- `text`: 回文部分文字列を数える文字列。
- 戻り値: 空でない回文部分文字列の出現数。同じ内容でも開始・終了位置が異なれば別々に数える。

## 時間計算量・空間計算量

$N$ を `text` の長さとする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `count_palindromic_substrings(text)` | $O(N)$ | $O(N)$ |

## 注意点

- 空文字列では `0` を返す。空の部分文字列は数えない。
- 文字の比較は大文字と小文字を区別する。
- 入力長は `int` に、答えは `long long` に収まる必要がある。
- 異なる回文の種類数ではなく、開始・終了位置で区別した出現数を返す。
