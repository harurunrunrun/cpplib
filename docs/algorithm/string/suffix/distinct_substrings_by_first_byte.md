---
title: Distinct Substrings by First Byte (先頭バイト別相異なる部分文字列数) [ADASTRNG]
documentation_of: ../../../../src/algorithm/string/suffix/distinct_substrings_by_first_byte.hpp
---

先頭のバイトごとに、空でない相異なる部分文字列の個数を集計する。

## API

~~~cpp
std::array<long long, 256>
distinct_substrings_by_first_byte(const std::string& text)
~~~

戻り値の添字 `byte` には、バイト `byte` で始まる空でない相異なる部分文字列の個数が入る。
空文字列では全要素が `0` になる。

## 時間計算量

$N$ を `text` の長さとする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `distinct_substrings_by_first_byte(text)` | $O(N+256)$ | $O(N+256)$ |

バイト単位の suffix automaton の遷移 DAG を状態の長さの降順に処理し、各状態から始まるパスの数を動的計画法で求める。
根からバイト `c` で遷移した先の状態から始まるパスの数が、`c` で始まる相異なる部分文字列の個数である。

## 注意点

- 文字は `unsigned char` に変換した $0$ 以上 $255$ 以下のバイト値で分類する。
- 入力長 $N$ は `(std::numeric_limits<int>::max() / 2) + 1` 以下でなければならない。
  超える場合は、状態を追加する前に `std::length_error` を送出する。
- 状態数は入力長 $1$ のとき $2$、それ以外の空でない入力では最大 $2N - 1$ である。
- 戻り値には空文字列を含めない。
