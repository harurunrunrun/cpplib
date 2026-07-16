---
title: Maximum Batman Cliff Count (特殊跳躍付き最大降下列長) [BAT3]
documentation_of: ../src/algorithm/other/maximum_batman_cliff_count.hpp
---

高さ列を右へ進む降下部分列の最大長を返す。

## maximum_batman_cliff_count

```cpp
int maximum_batman_cliff_count(const vector<int>& heights, int robin_index);
```

通常は次の高さが真に低い必要があるが、`robin_index` からの1歩だけは任意の高さへ進める。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_batman_cliff_count` | $O(N\log N)$ | $O(N)$ |

## 注意点

- `robin_index` は0始まりで範囲内でなければならない。
