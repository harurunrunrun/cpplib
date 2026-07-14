---
title: Longest Common Subsequence (最長共通部分列) [ALDS1_10_C]
documentation_of: ../src/algorithm/string/longest_common_subsequence.hpp
---

最長共通部分列。

# 関数

```cpp
int longest_common_subsequence_length(a, b)
Sequence longest_common_subsequence(a, b)
```

`longest_common_subsequence` は最長共通部分列を 1 つ返す。

## 時間計算量

- length: $O(NM)$
- 復元: $O(NM)$

## API別の時間計算量・空間計算量

$N=|a|$, $M=|b|$、復元するLCS長を $L$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `longest_common_subsequence_length(a,b)` | $O(NM)$ | $O(M)$ |
| `longest_common_subsequence(a,b)` | $O(NM+L)$ | $O(NM+L)$ |

復元版はDP表を前処理した後、$L$ 要素の結果列を構築する。

## 注意点

文字範囲、alphabet、固定容量、構築後の更新可否は各APIの説明に従う。返すindexと区間は0-indexedである。
