---
title: Edit Distance (編集距離) [DPL_1_E] [EDIST]
documentation_of: ../../../../src/algorithm/string/sequence/edit_distance.hpp
---

2 つの列の最小編集距離（重み付き）を DP で求める。

## 関数

```cpp
template<class Sequence>
int edit_distance(const Sequence& a, const Sequence& b,
                  int insert_cost = 1, int erase_cost = 1, int replace_cost = 1)
```

列 `a` を `b` に変換する最小コストを返す。挿入は `b` の要素を 1 つ加え、削除は `a` の要素を
1 つ除き、置換は異なる要素 1 つを置き換える操作である。等しい要素どうしを対応させるコストは 0 である。
要素は `operator==` で比較する。

## 時間計算量・空間計算量

$N=\lvert a\rvert$、$M=\lvert b\rvert$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `edit_distance(a, b, ...)` | $O(NM)$ | $O(M)$ |

## 注意点

コストの非負性や、結果の `int` オーバーフローは検査しない。
