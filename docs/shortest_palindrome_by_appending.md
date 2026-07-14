---
title: Shortest Palindrome by Appending (末尾追加による最短回文) [EPALIN]
documentation_of: ../src/algorithm/string/shortest_palindrome_by_appending.hpp
---

文字列の末尾だけに文字を追加して得られる最短の回文を返す。

## API

```cpp
string shortest_palindrome_by_appending(string_view text)
```

## 引数・戻り値

- `text`: 接頭辞として残す文字列。
- 戻り値: `text` の末尾へ0文字以上を追加して得られる最短の回文。
  追加列は一意で、すでに回文なら入力と同じ文字列、空文字列なら空文字列を返す。

## API別の時間計算量・空間計算量

$N=|text|$、返す回文の長さを $R$ とする。

| API | 時間計算量 | 空間計算量（出力・追加領域） |
| --- | --- | --- |
| `shortest_palindrome_by_appending(text)` | $O(N)$ | 出力 $O(R)$、補助 $O(N)$ |

$0\le R\le 2N-1$（ただし $N=0$ なら $R=0$）である。

## 注意点

- 比較は大文字と小文字を区別する。
- 空文字列では空文字列を返す。
- 文字集合に制限はなく、null文字を含む `string_view` も長さどおり扱う。
- 文字の変更や先頭への追加は行わず、元の `text` は返り値の接頭辞としてそのまま残る。
- 入力長は `int` に収まる必要がある。
