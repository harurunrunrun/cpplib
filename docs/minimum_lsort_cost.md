---
title: Minimum LSORT Cost (両端構築ソートの最小費用) [LSORT]
documentation_of: ../src/algorithm/other/minimum_lsort_cost.hpp
---

元列から要素を取り出して昇順列の左右へ追加する操作の最小総費用を返す。

## minimum_lsort_cost

```cpp
long long minimum_lsort_cost(const vector<int>& permutation);
```

第 `i` 手で残存列の `x` 番目を選ぶ費用を `i*x` とする。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_lsort_cost` | $O(N^2)$ | $O(N^2)$ |

## 注意点

- 入力は1からNの順列でなければならず、違反時は `runtime_error` を送出する。
