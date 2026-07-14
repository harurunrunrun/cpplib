---
title: Count Palindromic Substrings (回文部分文字列の個数) [NUMOFPAL]
documentation_of: ../src/algorithm/string/count_palindromic_substrings.hpp
---

文字列に含まれるすべての非空な回文部分文字列を、出現位置ごとに数える。
Manacher 法で求めた各中心の奇数・偶数回文半径の総和を返す。

# count_palindromic_substrings

```cpp
long long count_palindromic_substrings(const string& text)
```

`text` の回文部分文字列の出現数を返す。同じ内容でも区間が異なれば別々に数える。
例えば `"aaa"` では長さ1が3個、長さ2が2個、長さ3が1個なので `6` を返す。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `count_palindromic_substrings(text)` | $O(N)$ | $O(N)$ |

## 注意点

- 空文字列では `0` を返す。
- 空部分文字列は数えない。
- 文字の比較は大文字と小文字を区別する。
- 入力長は `int` に収まり、答えは `long long` に収まる必要がある。
- 異なる回文文字列の種類数ではなく、開始・終了位置で区別した出現数を返す。
