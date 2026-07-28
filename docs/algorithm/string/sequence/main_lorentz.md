---
title: Main-Lorentz Algorithm (Main-Lorentz法)
documentation_of: ../../../../src/algorithm/string/sequence/main_lorentz.hpp
---

文字列に出現する全tandem repeat $XX$ を、runとその周期の倍数から列挙する。

## API

```cpp
std::vector<TandemRepeat> main_lorentz_tandem_repeats(
    const std::string& text
)
```

`TandemRepeat` は `begin` と `half_length=|X|` を持つ。同じ出現は1回だけ返す。

## API別の時間計算量・空間計算量

$N=|text|$、生成候補数を $C$、異なる出現数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `main_lorentz_tandem_repeats` | $O(N+C+K\log K)$ | $O(N+C)$ |

出力自体が $\Theta(N^2)$ 件になる文字列がある。

## 注意点

全出現を返すため、出力件数に比例する時間とメモリは避けられない。
