---
title: Main-Lorentz Algorithm (Main-Lorentz法)
documentation_of: ../../../../src/algorithm/string/sequence/main_lorentz.hpp
---

文字列を半分ずつ再帰分割し、境界をまたぐtandem repeat $XX$ を4本の
Z配列から列挙するMain--Lorentz法。

## API

```cpp
std::vector<TandemRepeat> main_lorentz_tandem_repeats(
    const std::string& text
)
```

`TandemRepeat` は `begin` と `half_length=|X|` を持つ。同じ出現は1回だけ返す。

## API別の時間計算量・空間計算量

$N=|text|$、異なる出現数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `main_lorentz_tandem_repeats` | $O(N\log N+K\log K)$ | $O(N+K)$ |

出力自体が $\Theta(N^2)$ 件になる文字列がある。

## 注意点

全出現を返すため、出力件数に比例する時間とメモリは避けられない。
入力は空文字列を含む任意のバイト列を受理し、結果は
`(begin, half_length)` の辞書順で重複なく返す。
