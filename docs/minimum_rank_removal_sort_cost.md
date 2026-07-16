---
title: Minimum Rank-Removal Sort Cost (順位費用付き除去ソートの最小費用) [LSORT]
documentation_of: ../src/algorithm/other/minimum_rank_removal_sort_cost.hpp
---

順列から要素を1個ずつ除去し、除去順を値の昇順または降順側から構成するときの最小総費用を求める。

## minimum_rank_removal_sort_cost

```cpp
long long minimum_rank_removal_sort_cost(
    const vector<int>& permutation
);
```

未除去の値が連続区間 $[l,r]$ を成す状態から、値 $l$ または $r$ を除去する。現在の未除去列で選んだ要素が左から $x$ 番目、現在までに確定する列の長さが $k$ のとき、その操作費用を $kx$ とする。全要素を除去する最小総費用を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_rank_removal_sort_cost` | $O(N^2)$ | $O(N^2)$ |

## 注意点

- 入力は1から $N$ を各1回含む順列でなければならない。違反時は `runtime_error` を送出する。
- 空の順列には0を返す。
- 配列長を内部添字で表せない場合は `length_error`、結果が `long long` を超える場合は `overflow_error` を送出する。
