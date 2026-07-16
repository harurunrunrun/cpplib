---
title: Maximum Consecutive Repetitions (最大連続反復回数) [REPEATS]
documentation_of: ../src/algorithm/string/maximum_consecutive_repetitions.hpp
---

文字列の任意の部分文字列に現れる、連続した同一ブロックの最大個数を求める。

## API

```cpp
int maximum_consecutive_repetitions(const std::string& text)
```

2種類の辞書順について各位置から始まる最長 Lyndon 語を調べ、run の根になり得る
$O(N)$ 個の候補だけを列挙する。候補の左右への延長量は suffix array と LCP 配列で
求める。LCP の区間最小値は Cartesian tree の Euler tour と $\pm1$ RMQ で処理するため、
前処理と全候補の検査を合わせても線形時間となる。

## 引数・戻り値

- `text`: 反復を調べる byte 文字列。
- 戻り値: `text` の部分文字列を $T^k$（非空文字列 $T$ を $k$ 回連結した列）
  と表せる最大の $k$。空文字列では `0`、非空文字列では少なくとも `1` を返す。
  各 $T$ は重ならず連続する。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_consecutive_repetitions(text)` | $O(N)$ | $O(N)$ |

## 注意点

- `char` の符号には依存せず、0 から 255 までの byte 値を扱える。
- 返すのは反復回数のみで、種文字列や開始位置は返さない。
- `text.size() > (INT_MAX + 3) / 2` の場合は、内部の Euler tour を
  `int` で添字付けできないため `std::length_error` を送出する。
