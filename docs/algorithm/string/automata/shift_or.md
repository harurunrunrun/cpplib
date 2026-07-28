---
title: Shift-Or Algorithm (Shift-Or法)
documentation_of: ../../../../src/algorithm/string/automata/shift_or.hpp
---

不一致を0 bitで表すbit並列オートマトンにより、パターンの完全一致位置を列挙する。

## API

```cpp
std::vector<int> shift_or_match(
    std::string_view text, std::string_view pattern
)
```

空パターンではすべての境界位置を返す。64文字を超えるパターンも複数wordで扱う。

## API別の時間計算量・空間計算量

$N=|text|,\ M=|pattern|,\ W=\lceil M/64\rceil$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `shift_or_match` | $O((N+256)W+K)$ | $O(256W+K)$ |

$K$ は一致件数である。

## 注意点

返す位置は0-indexedであり、重なっている一致もすべて列挙する。
