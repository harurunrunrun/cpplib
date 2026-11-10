---
title: Main-Lorentz Algorithm (Main-Lorentz法)
documentation_of: ../../../../src/algorithm/string/sequence/main_lorentz.hpp
---

文字列を半分ずつ再帰的に分割し、境界をまたぐ平方（tandem repeat）$XX$ を 4 本の
Z 配列から列挙する Main--Lorentz 法である。

## API

```cpp
struct TandemRepeat {
    int begin;
    int half_length;
};

std::vector<TandemRepeat> main_lorentz_tandem_repeats(
    const std::string& text
)
```

`begin` は $XX$ の開始位置、`half_length` は $\lvert X\rvert$ である。同じ出現は 1 回だけ返す。
結果は `(begin, half_length)` の辞書順に並ぶ。

## 時間計算量・空間計算量

$N=\lvert \mathrm{text}\rvert$、相異なる出現の個数を $K$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `main_lorentz_tandem_repeats` | $O(N\log N+K\log K)$ | $O(N+K)$ |

出力自体が $\Theta(N^2)$ 件になる文字列（例えば同じ文字の繰り返し）がある。

## 注意点

- 全出現を返すため、出力件数に比例する時間とメモリは避けられない。
- 入力は空文字列を含む、長さ `INT_MAX` 以下の任意のバイト列を受け付ける。長さが上限を超える場合は `std::length_error` を送出する。
