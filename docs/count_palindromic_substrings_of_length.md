---
title: Count Fixed-Length Palindromic Substrings (固定長回文部分文字列の個数) [PLD]
documentation_of: ../src/algorithm/string/palindrome/count_palindromic_substrings_of_length.hpp
---

指定した長さの回文部分文字列を、開始位置ごとに数える。

## API

```cpp
long long count_palindromic_substrings_of_length(
    const string& text,
    int length
)
```

## 引数・戻り値

- `text`: 回文部分文字列を数える文字列。
- `length`: 対象の部分文字列長。
- 戻り値: 長さ `length` の回文部分文字列の出現数。同じ内容でも開始位置が
  異なれば別々に数える。`length <= 0` または `length > text.size()` なら
  `0` を返す。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `count_palindromic_substrings_of_length(text, length)` | $O(N)$ | $O(N)$ |

## 注意点

- `length <= 0` または `length > text.size()` なら `0` を返す。
- 文字の比較は大文字と小文字を区別する。
- 入力長は `int` に収まる必要がある。
- 戻り値は開始位置の個数であり、異なる内容の回文の種類数ではない。
