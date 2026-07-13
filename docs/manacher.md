---
title: Manacher
documentation_of: ../src/algorithm/string/manacher.hpp
---

各中心の回文半径を求める。

## 関数

- `manacher_odd(s)`
    - `i` を中心とする奇数長回文の半径。
- `manacher_even(s)`
    - `i - 1` と `i` の間を中心とする偶数長回文の半径。
- `longest_palindrome(s)`
    - 最長回文部分文字列の `(開始位置, 長さ)`。

## 計算量

- $O(n)$

## API別計算量

| API | 時間計算量 | 出力・追加領域 |
| --- | --- | --- |
| `manacher_odd(s)` | $O(N)$ | $O(N)$ |
| `manacher_even(s)` | $O(N)$ | $O(N)$ |
| `longest_palindrome(s)` | $O(N)$ | 補助 $O(N)$、戻り値 $O(1)$ |

前2関数は全 $N$ 中心の半径を列挙する。
