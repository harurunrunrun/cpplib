---
title: Minimum Binary Substring Mismatches (二進文字列の部分文字列最小不一致数) [ABC196_F]
documentation_of: ../../../../src/algorithm/string/automata/minimum_binary_substring_mismatches.hpp
---

二進文字列 `text` の長さ `pattern.size()` の各部分文字列と `pattern` との、不一致文字数の最小値を返す。
NTT による畳み込みで全位置の不一致数を求める。

## 関数

```cpp
int minimum_binary_substring_mismatches(
    const string& text,
    const string& pattern
)
```

`text.substr(left, pattern.size())` と `pattern` のハミング距離を、
`0 <= left <= text.size() - pattern.size()` の範囲で最小化した値を返す。
`pattern` が空なら 0 を返す。

## 時間計算量・空間計算量

$N=\lvert \mathrm{text}\rvert$、$M=\lvert \mathrm{pattern}\rvert$ とし、$Z$ を $N+M-1$ 以上の最小の 2 冪とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_binary_substring_mismatches(text, pattern)` | $O(Z\log Z)$ | $O(Z)$ |

## 注意点

- `text` と `pattern` の各文字は `0` または `1` でなければならない。
- `pattern.size() <= text.size()` でなければならない。
- 空でない入力では $N+M-1\le 2^{23}$ でなければならない。
- 前提に違反すると `std::runtime_error` を送出する。
