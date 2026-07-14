---
title: Gondor Spark Lighting Times (ゴンドール烽火点火時刻) [GONDOR]
documentation_of: ../src/algorithm/graph/gondor_spark_lighting_times.hpp
---

各烽火台の座標・矢数・射撃優先順から、全烽火台が初めて点火する時刻を返す。

# GondorSpark

```cpp
struct GondorSpark{
    long long x;
    long long y;
    int arrow_count;
    vector<int> instruction_order;
};
```

烽火台の座標、点火直後に射てる矢数、ほかの烽火台を検討する順序を保持する。

# gondor_spark_lighting_times

```cpp
vector<double> gondor_spark_lighting_times(
    const vector<GondorSpark>& sparks
);
```

烽火台 `0` が時刻 `0` に点火済みであるとして、各烽火台の初回点火時刻を番号順に返す。

## API別の時間計算量・空間計算量

烽火台数を $N$、実際に射られる矢の総数を $A$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `gondor_spark_lighting_times` | $O(N^2 + A\log A)$ | $O(N^2 + A)$ |

## 注意点

- 各 `instruction_order` は自分以外の全烽火台をちょうど1回ずつ含まなければならない。
- `arrow_count` は非負でなければならず、烽火台 `0` から最終的に全烽火台が点火可能でなければならない。
- 前提違反時は `runtime_error` を送出する。
