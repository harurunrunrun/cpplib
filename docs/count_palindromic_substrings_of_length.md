---
title: Count Fixed-Length Palindromic Substrings (固定長回文部分文字列の個数) [PLD]
documentation_of: ../src/algorithm/string/count_palindromic_substrings_of_length.hpp
---

指定した長さの回文部分文字列を、開始位置ごとに数える。
内部では Manacher 法の奇数・偶数回文半径のうち必要な側だけを計算する。

# count_palindromic_substrings_of_length

```cpp
long long count_palindromic_substrings_of_length(
    const string& text,
    int length
)
```

`text` に含まれる長さ `length` の回文部分文字列の個数を返す。
同じ文字列でも開始位置が異なれば別々に数える。

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
