---
title: Maximum Commando Effectiveness (部隊分割の最大戦闘力) [APIO10A]
documentation_of: ../src/algorithm/other/maximum_commando_effectiveness.hpp
---

正の値の列を連続区間へ分割したときの評価値合計の最大値を返す。

## maximum_commando_effectiveness

```cpp
long long maximum_commando_effectiveness(
    const vector<int>& effectiveness,
    long long quadratic, long long linear, long long constant
);
```

各区間和を $x$ とした評価 `quadratic*x*x + linear*x + constant` の合計を最大化する。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_commando_effectiveness` | $O(N)$ | $O(N)$ |

## 注意点

- `quadratic` は負、各要素は正でなければならない。
- 前提違反は `runtime_error`、算術範囲違反は `overflow_error` となる。
