---
title: Distinct Substrings by First Byte (先頭バイト別相異なる部分文字列数) [ADASTRNG]
documentation_of: ../src/algorithm/string/suffix/distinct_substrings_by_first_byte.hpp
---

先頭バイトごとに、空でない相異なる部分文字列数を集計する。

## API

~~~cpp
array<long long, 256>
distinct_substrings_by_first_byte(const string& text)
~~~

戻り値の添字 `byte` には、`byte` で始まる相異なる非空部分文字列数が入る。
空文字列では全要素が `0` になる。

## 時間計算量

$N=|text|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `distinct_substrings_by_first_byte(text)` | $O(N+256)$ | $O(N+256)$ |

byte suffix automatonの遷移DAGを状態長の降順に処理し、各状態から始まるpath数を
動的計画法で求める。rootからbyte `c` で遷移した先のpath数が、`c` で始まる
相異なる部分文字列数である。

## 注意点

- 文字は `unsigned char` に変換した `0` 以上 `255` 以下のバイト値で分類する。
- 入力長は `int` で表現できなければならず、超える場合は
  `std::length_error` を送出する。
- 戻り値には空文字列を含めない。
