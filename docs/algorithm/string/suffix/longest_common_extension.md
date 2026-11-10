---
title: Longest Common Extension (最長共通延長)
documentation_of: ../../../../src/algorithm/string/suffix/longest_common_extension.hpp
---

接尾辞配列と、LCP 配列上の Sparse Table による RMQ を用いて、任意の 2 つの接尾辞の最長共通接頭辞長を求める。

## API

```cpp
explicit LongestCommonExtension(const std::string& text)
int lce(int first, int second) const
```

`lce(first, second)` は位置 `first` と `second` から始まる接尾辞の最長共通接頭辞長を返す。
`first` と `second` には末尾位置 $N$ も指定でき、その接尾辞は空文字列である。
位置が $[0, N]$ の範囲外の場合は `std::runtime_error` を送出する。

## API 別の時間計算量・空間計算量

$N$ を `text` の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `lce` | $O(1)$ | $O(1)$ |

## 注意点

前処理後に文字列を変更することはできない。
