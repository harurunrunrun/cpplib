---
title: Maximum Consecutive Repetitions (最大連続反復回数) [REPEATS]
documentation_of: ../../../../src/algorithm/string/sequence/maximum_consecutive_repetitions.hpp
---

文字列の部分文字列として現れる、同一ブロックの連続した繰り返しの最大個数を求める。

## API

```cpp
int maximum_consecutive_repetitions(const std::string& text)
```

2 種類の辞書順について各位置から始まる最長の Lyndon 語を調べ、run の根になりうる
$O(N)$ 個の候補だけを列挙する。候補の左右への延長量は接尾辞配列と LCP 配列で
求める。LCP の区間最小値は Cartesian tree の Euler tour と $\pm1$ RMQ で処理するため、
前処理と全候補の検査を合わせても線形時間となる。

## 引数・戻り値

- `text`: 繰り返しを調べるバイト文字列。
- 戻り値: `text` のある部分文字列を $T^k$（空でない文字列 $T$ を重ならずに $k$ 回連結したもの）
  と表せる最大の $k$。空文字列では `0`、空でない文字列では `1` 以上を返す。

## 時間計算量・空間計算量

$N=\lvert \mathrm{text}\rvert$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_consecutive_repetitions(text)` | $O(N)$ | $O(N)$ |

## 注意点

- `char` の符号に依存せず、0 から 255 までのバイト値を扱える。
- 返すのは繰り返し回数だけで、ブロックの文字列や開始位置は返さない。
- `text.size() > (INT_MAX + 3) / 2` の場合は、内部の Euler tour を
  `int` で添字付けできないため `std::length_error` を送出する。
