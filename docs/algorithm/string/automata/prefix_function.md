---
title: Prefix Function / KMP (接頭辞関数・KMP法) [ALDS1_14_B]
documentation_of: ../../../../src/algorithm/string/automata/prefix_function.hpp
---

文字列または列の接頭辞関数（prefix function）と、KMP 法によるパターン検索である。

```cpp
auto failure = prefix_function(sequence);
auto positions = kmp_search(text, pattern);
```

- `prefix_function(sequence)[i]` は、`sequence` の先頭 $i+1$ 要素（閉区間 `[0, i]`）について、その接頭辞かつ接尾辞である最長の真部分列の長さを返す。
- `kmp_search(text, pattern)` はパターンが現れる開始位置を昇順に返す。重なる出現も列挙する。空パターンは `0, 1, ..., text.size()` のすべてに現れるものとする。

## 時間計算量・空間計算量

テキスト長を $N$、パターンまたは入力列の長さを $M$、出現数を $K$ とし、要素の比較 1 回を $O(1)$ とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `prefix_function(sequence)` | $O(M)$ | $O(M)$ |
| `kmp_search(text, pattern)` | $O(N+M+K)$ | $O(M+K)$ |

## 注意点

- 列は `size()`、0-indexed のランダムアクセス、要素の `operator==` に対応している必要がある。`kmp_search` ではテキストとパターンの要素同士が `operator==` で比較できればよい。
- 返す位置は 0-indexed であり、長さは `int` に収まることを前提とする。
