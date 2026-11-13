---
title: Shortest Palindrome by Appending (末尾追加による最短回文) [EPALIN]
documentation_of: ../../../../src/algorithm/string/palindrome/shortest_palindrome_by_appending.hpp
---

文字列の末尾だけに文字を追加して得られる最短の回文を返す。

## API

```cpp
string shortest_palindrome_by_appending(string_view text)
```

## 引数・戻り値

- `text`: 接頭辞として残す文字列。
- 戻り値: `text` の末尾に 0 文字以上を追加して得られる最短の回文。
  追加する列は一意に定まる。すでに回文なら入力と同じ文字列を、空文字列なら空文字列を返す。

## 時間計算量・空間計算量

$N=\lvert \mathrm{text}\rvert$、返す回文の長さを $R$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `shortest_palindrome_by_appending(text)` | $O(N)$ | 出力 $O(R)$、補助 $O(N)$ |

$N\ge 1$ なら $N\le R\le 2N-1$、$N=0$ なら $R=0$ である。

## 注意点

- 比較は大文字と小文字を区別する。
- 文字集合に制限はなく、NUL 文字を含む `string_view` も長さどおりに扱う。
- 文字の変更や先頭への追加は行わず、元の `text` は返り値の接頭辞としてそのまま残る。
- 入力長は `int` に収まる必要がある。
