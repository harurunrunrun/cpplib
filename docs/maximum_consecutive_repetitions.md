---
title: Maximum Consecutive Repetitions (最大連続反復回数) [REPEATS]
documentation_of: ../src/algorithm/string/maximum_consecutive_repetitions.hpp
---

文字列中の連続する同一ブロックの最大個数を求める。

## API

```cpp
int maximum_consecutive_repetitions(const std::string& text)
```

`text` の部分文字列を $T^k$（非空文字列 $T$ を $k$ 回連結した文字列）と
表せる最大の $k$ を返す。空文字列では `0`、非空文字列では少なくとも `1` を返す。
反復部分文字列は重なりを持たず、`text` 内で連続している必要がある。

## 時間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_consecutive_repetitions(text)` | $O(N\log N)$ | $O(N\log N)$ |

$N=|text|$ とする。順方向・逆方向の suffix array と LCP の RMQ を構築し、
各周期について調和級数個の境界を調べる。

## 注意点

byte 単位で文字を比較する。返すのは反復回数のみで、種文字列や開始位置は返さない。
