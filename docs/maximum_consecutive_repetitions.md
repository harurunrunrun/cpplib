---
title: Maximum Consecutive Repetitions (最大連続反復回数) [REPEATS]
documentation_of: ../src/algorithm/string/maximum_consecutive_repetitions.hpp
---

文字列中の連続する同一ブロックの最大個数を求める。

## API

```cpp
int maximum_consecutive_repetitions(const std::string& text)
```

## 引数・戻り値

- `text`: 反復を調べる文字列。
- 戻り値: `text` の部分文字列を $T^k$（非空文字列 $T$ を $k$ 回連結した列）
  と表せる最大の $k$。空文字列では `0`、非空文字列では少なくとも `1` を返す。
  各 $T$ は重ならず連続する。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_consecutive_repetitions(text)` | $O(N\log N)$ | $O(N\log N)$ |

## 注意点

- byte単位で文字を比較する。
- 返すのは反復回数のみで、種文字列や開始位置は返さない。
- `text.size()` は `int` に収まる必要がある。
