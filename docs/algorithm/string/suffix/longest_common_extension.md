---
title: Longest Common Extension (最長共通延長)
documentation_of: ../../../../src/algorithm/string/suffix/longest_common_extension.hpp
---

接尾辞配列とLCP配列のRMQにより、任意の2接尾辞の最長共通接頭辞長を求める。

## API

```cpp
LongestCommonExtension(const std::string& text)
int lce(int first, int second) const
```

`first` と `second` は末尾位置 $N$ も指定でき、その接尾辞は空文字列である。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `lce` | $O(1)$ | $O(1)$ |

範囲外の位置では例外を送出する。

## 注意点

前処理後の文字列を変更することはできない。
