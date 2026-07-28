---
title: Dictionary of Basic Factors (基本因子辞書)
documentation_of: ../../../../src/algorithm/string/suffix/dictionary_of_basic_factors.hpp
---

長さ $2^k$ の全部分文字列を同値類IDで管理する。任意長の部分文字列は先頭・末尾の
2基本因子で比較でき、2接尾辞のLCEは大きい基本因子から貪欲に延長して求める。

## API

```cpp
DictionaryOfBasicFactors(const std::string& text)
int size() const
int rank(int position, int level) const
bool equal(int first, int second, int length) const
int lce(int first, int second) const
```

`rank(position,level)` は長さ $2^{level}$ の因子IDを返す。範囲外では例外を送出する。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `size`, `rank` | $O(1)$ | $O(1)$ |
| `equal` | $O(1)$ | $O(1)$ |
| `lce` | $O(\log N)$ | $O(1)$ |

## 注意点

空文字列ではlevelを持たず、`rank` は呼び出せない。
