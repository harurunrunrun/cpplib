---
title: Run Enumeration (Run列挙) [runenumerate]
documentation_of: ../../../../src/algorithm/string/sequence/run_enumeration.hpp
---

run（周期を保ったまま左右に延長できない、長さが最小周期の 2 倍以上の極大周期区間）をすべて列挙する。
2 種類の辞書順における Lyndon 根の候補を LCE で左右に延長する。

## API

```cpp
struct StringRun {
    int left;
    int right;
    int period;
};

std::vector<StringRun> enumerate_runs(const std::string& text)
```

`StringRun` は半開区間 `[left, right)` と最小周期 `period` を持つ。
返り値はこの 3 値の辞書順に並び、重複を除いている。

## 時間計算量・空間計算量

$N=\lvert \mathrm{text}\rvert$、run の個数を $R$ とする。run の個数は $O(N)$ である。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `enumerate_runs` | $O(N+R\log R)$ | $O(N+R)$ |

## 注意点

区間は半開区間であり、周期はその区間の最小周期である。
