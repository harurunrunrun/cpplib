---
title: Edit Distance (編集距離) [DPL_1_E]
documentation_of: ../src/algorithm/string/edit_distance.hpp
---

最小編集距離。

# 関数

```cpp
int edit_distance(a, b, insert_cost = 1, erase_cost = 1, replace_cost = 1)
```

挿入、削除、置換のコストを指定できる。

## 時間計算量

- $O(NM)$

## API別の時間計算量・空間計算量

$N=|a|$, $M=|b|$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `edit_distance(a,b,...)` | $O(NM)$ | $O(M)$ |

各挿入・削除・置換候補の評価は $O(1)$ とする。
## API詳細

`edit_distance(a,b,insert_cost,erase_cost,replace_cost)` は列 `a` を `b` へ
変換する最小コストを返す。挿入は `b` の要素を1つ加え、削除は `a` の要素を
1つ除き、置換は異なる要素1つを置き換える。同じ要素の対応コストは0。
要素は `operator==` で比較する。コストの非負性や結果の `int` overflowは
検査しない。

## 注意点

文字範囲、alphabet、固定容量、構築後の更新可否は各APIの説明に従う。返すindexと区間は0-indexedである。
