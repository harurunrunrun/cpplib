---
title: Prefix Function / KMP (接頭辞関数・KMP法) [ALDS1_14_B]
documentation_of: ../src/algorithm/string/automata/prefix_function.hpp
---

文字列または列のprefix functionと、KMPによるpattern検索。

```cpp
auto failure = prefix_function(sequence);
auto positions = kmp_search(text, pattern);
```

`prefix_function(sequence)[i]` は、`sequence[0, i]` の接頭辞かつ接尾辞である
最長の真部分列の長さを返す。`kmp_search` はpatternが現れる開始位置を昇順で返し、
重なる出現も列挙する。空patternは `0, 1, ..., text.size()` に現れるものとする。

## API別の時間計算量・空間計算量

text長を $N$、patternまたは入力列の長さを $M$、出現数を $K$ とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `prefix_function(sequence)` | $O(M)$ | $O(M)$ |
| `kmp_search(text, pattern)` | $O(N+M+K)$ | $O(M+K)$ |

要素比較1回の計算量は $O(1)$ とする。

## 注意点

文字範囲、alphabet、固定容量、構築後の更新可否は各APIの説明に従う。返すindexと区間は0-indexedである。
