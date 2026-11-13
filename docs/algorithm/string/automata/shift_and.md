---
title: Shift-And Algorithm (Shift-And法)
documentation_of: ../../../../src/algorithm/string/automata/shift_and.hpp
---

一致状態を 1 のビットで表すビット並列オートマトン（Shift-And 法）により、パターンの完全一致位置を列挙する。

## API

```cpp
std::vector<int> shift_and_match(
    std::string_view text, std::string_view pattern
)
```

パターンが現れる 0-indexed の開始位置を昇順に返す。
空パターンでは `0, 1, ..., text.size()` のすべての位置を返す。64 文字を超えるパターンも複数ワードで扱う。

## 時間計算量・空間計算量

$N$ をテキスト長、$M$ をパターン長、$W=\lceil M/64\rceil$、$K$ を一致件数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `shift_and_match` | $O((N+256)W+K)$ | $O(256W+K)$ |

## 注意点

重なっている一致もすべて列挙する。文字はバイト（`unsigned char`）として扱う。
