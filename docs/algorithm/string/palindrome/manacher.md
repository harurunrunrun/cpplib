---
title: Manacher (Manacher法)
documentation_of: ../../../../src/algorithm/string/palindrome/manacher.hpp
---

Manacher 法により、各中心の回文半径を求める。

## 関数

```cpp
std::vector<int> manacher_odd(const std::string& s)
std::vector<int> manacher_even(const std::string& s)
std::pair<int, int> longest_palindrome(const std::string& s)
```

- `manacher_odd(s)[i]`: 文字 `i` を中心とする最長の奇数長回文の半径（中心を含む）。長さは `2 * d - 1` である。
- `manacher_even(s)[i]`: 文字 `i - 1` と `i` の間を中心とする最長の偶数長回文の半径。長さは `2 * d` である（`i = 0` では 0）。
- `longest_palindrome(s)`: 最長の回文部分文字列の `(開始位置, 長さ)`。

## 時間計算量・空間計算量

$N$ を `s` の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `manacher_odd(s)` | $O(N)$ | $O(N)$ |
| `manacher_even(s)` | $O(N)$ | $O(N)$ |
| `longest_palindrome(s)` | $O(N)$ | 補助 $O(N)$、戻り値 $O(1)$ |

## 注意点

- 返す添字は 0-indexed である。
- 文字の比較は大文字と小文字を区別する。長さは `int` に収まることを前提とする。
