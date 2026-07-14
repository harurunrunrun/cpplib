---
title: Minimum Circle Interval Cover (円周区間最小被覆)
documentation_of: ../src/algorithm/geometry/2d/minimum_circle_interval_cover.hpp
---

円周上の候補区間から、全周を覆う最小本数を求める。

# API

~~~cpp
int minimum_circle_interval_cover(
    const vector<pair<long double, long double>>& intervals
)
~~~

各pairはstartからendへ反時計回りに進む半開区間を表す。全周を覆えない場合は-1、覆える場合は必要な候補区間の最小本数を返す。同じ候補を複数回選ぶことはできない。

# 時間計算量

候補数をNとするとO(N log(N+1))。区間を二重化してsortし、貪欲遷移をbinary liftingする。

# 空間計算量

O(N log(N+1))。

## 注意点

- 端点はradianで指定し、任意の2π周期表現を受け付ける。非有限値があればinvalid_argument。
- abs(end-start)が2π-GEOMETRY_EPS以上の候補は全周区間であり、答えは1。
- 正規化した長さがGEOMETRY_EPS以下の候補は空区間として無視する。
- wrapする区間と端点共有を扱う。被覆判定はGEOMETRY_EPSの許容誤差を用いる。
