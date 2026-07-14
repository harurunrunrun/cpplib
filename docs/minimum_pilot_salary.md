---
title: Minimum Pilot Salary (操縦士ペアの最小給与) [MPILOT]
documentation_of: ../src/algorithm/other/minimum_pilot_salary.hpp
---

年齢順の操縦士を機長と副操縦士のペアへ割り当てる最小給与を返す。

# PilotSalaries

```cpp
struct PilotSalaries { long long captain, assistant; };
```

# minimum_pilot_salary

```cpp
long long minimum_pilot_salary(const vector<PilotSalaries>& pilots_by_increasing_age);
```

各ペアの副操縦士が機長より若くなる割当てを対象とする。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_pilot_salary` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 人数は偶数、給与は非負でなければならない。
