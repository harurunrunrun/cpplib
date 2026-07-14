---
title: Shortest Palindrome by Appending (末尾追加による最短回文) [EPALIN]
documentation_of: ../src/algorithm/string/shortest_palindrome_by_appending.hpp
---

文字列の末尾だけに文字を追加して得られる最短の回文を返す。
既存の Manacher 法を再利用して最長回文接尾辞を求め、その直前の部分を逆順に追加する。

# shortest_palindrome_by_appending

```cpp
string shortest_palindrome_by_appending(string_view text)
```

`text` を接頭辞として持ち、末尾への0文字以上の追加だけで作れる最短の回文を返す。
追加する文字列は一意に定まる。

例えば `"abcd"` には `"cba"` を追加して `"abcdcba"` を返す。
すでに回文なら入力と同じ文字列を返す。

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
- 内部で `manacher_odd` と `manacher_even` を順に実行する。
