---
title: Maximum Treat Revenue (おやつ販売の最大収益) [TRT]
documentation_of: ../src/algorithm/other/maximum_treat_revenue.hpp
---

両端から1個ずつ販売する列の最大収益を返す。

# maximum_treat_revenue

```cpp
long long maximum_treat_revenue(const vector<int>& values);
```

第 `day` 日に売る要素の収益を `day*value` として全要素を売る最大収益を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_treat_revenue` | $O(N^2)$ | $O(N)$ |

## 注意点

- 値は非負でなければならず、違反時は `runtime_error` を送出する。
