---
title: Dictionary of Basic Factors (基本因子辞書)
documentation_of: ../../../../src/algorithm/string/suffix/dictionary_of_basic_factors.hpp
---

長さ $2^k$ のすべての部分文字列（基本因子）に同値類 ID を割り当てて管理する。
任意長の部分文字列は先頭と末尾の 2 つの基本因子で比較でき、2 つの接尾辞の LCE は大きい基本因子から貪欲に延長して求める。

## API

```cpp
explicit DictionaryOfBasicFactors(const std::string& text)
int size() const
int rank(int position, int level) const
bool equal(int first, int second, int length) const
int lce(int first, int second) const
```

- `size()` は `text` の長さを返す。
- `rank(position, level)` は位置 `position` から始まる長さ $2^{\mathrm{level}}$ の因子の ID を返す。ID は同じ長さの因子の間で辞書順を保つ。`level` または `position` が範囲外の場合は `std::runtime_error` を送出する。
- `equal(first, second, length)` は `text.substr(first, length)` と `text.substr(second, length)` が等しいかを返す。`length < 0` または部分文字列が範囲外の場合は `std::runtime_error` を送出する。`length == 0` なら `true` を返す。
- `lce(first, second)` は位置 `first` と `second` から始まる接尾辞の最長共通接頭辞長を返す。`first`, `second` は $[0, N]$ の範囲でなければならず、範囲外では `std::runtime_error` を送出する。

各文字は `unsigned char` として扱う。

## API 別の時間計算量・空間計算量

$N$ を `text` の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `size`, `rank` | $O(1)$ | $O(1)$ |
| `equal` | $O(1)$ | $O(1)$ |
| `lce` | $O(\log N)$ | $O(1)$ |

## 注意点

空文字列では level を 1 つも持たないため、`rank` は呼び出せない（常に例外になる）。
