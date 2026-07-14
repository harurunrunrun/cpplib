---
title: Fraction Tree Nth Node (分数木の第N頂点) [NG0FRCTN]
documentation_of: ../src/algorithm/math/fraction_tree_nth.hpp
---

Calkin-Wilf型の二分木を幅優先順に番号付けし、指定頂点の既約分数を返す。

# `fraction_tree_nth`

```cpp
pair<uint64_t,uint64_t> fraction_tree_nth(uint64_t one_based_index);
```

根を `1/1`、`i/j` の左子を `i/(i+j)`、右子を `(i+j)/j` とした木の指定頂点を `{分子,分母}` で返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `fraction_tree_nth` | $O(\log one\_based\_index)$ | $O(1)$ |

## 注意点

- 添字は1以上でなければならない。
