---
title: Count Morse Decodings (モールス信号復号数) [MORSE]
documentation_of: ../src/algorithm/string/count_morse_decodings.hpp
---

# count_morse_decodings

```cpp
long long count_morse_decodings(
    const string& signal,
    const vector<string>& words,
    long long count_limit = numeric_limits<long long>::max()
)
```

## 引数

- `signal`: `.` と `-` からなる信号。
- `words`: 大文字英字からなる辞書。重複語は別の選択肢として数える。
- `count_limit`: 戻り値の上限。

## 戻り値

辞書語の列を連結して `signal` と一致させる方法数と `count_limit` の小さい方。

## API別の時間計算量・空間計算量

信号長を $S$、辞書語をモールス符号化した総長を $D$、最大符号長を $L$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_morse_decodings` | $O(D+SL)$ | $O(D+S)$ |

## 注意点

- 空の辞書語、信号・単語の不正文字、負の `count_limit` では `invalid_argument` を送出する。
- 辞書node数または同一語数を内部整数で表現できない場合は例外を送出する。
