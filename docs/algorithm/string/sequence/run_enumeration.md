---
title: Run Enumeration (Run列挙)
documentation_of: ../../../../src/algorithm/string/sequence/run_enumeration.hpp
---

周期を保ったまま左右へ延長できない、長さが周期の2倍以上の極大周期区間を列挙する。
2種類の辞書順におけるLyndon root候補をLCEで左右に延長する。

## API

```cpp
std::vector<StringRun> enumerate_runs(const std::string& text)
```

`StringRun` は半開区間 `left`, `right` と最小周期 `period` を持つ。
返り値はこの3値の辞書順で重複を除いている。

## API別の時間計算量・空間計算量

$N=|text|$、run数を $R$ とする。run数は $O(N)$ である。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `enumerate_runs` | $O(N+R\log R)$ | $O(N+R)$ |

## 注意点

区間は半開区間であり、周期はその区間の最小周期である。
