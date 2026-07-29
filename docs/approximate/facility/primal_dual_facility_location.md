---
title: Primal-Dual Facility Location (主双対施設配置)
documentation_of: ../../../src/approximate/facility/primal_dual_facility_location.hpp
---

非容量制約 facility location の Jain--Vazirani 型 primal-dual heuristic。
`approximate::facility` 名前空間に属する。

## API

```cpp
primal_dual_facility_location(
    facility_count, client_count, opening_cost, distance);

primal_dual_facility_location(distance_matrix, opening_cost);
```

前者の `distance(facility, client)` は非負有限値を返す。active client の dual
を同じ速度で増加させ、client contribution の和が opening cost に達した
facility を tight にする。tight facility に寄与する client を停止し、全 client
停止後に共通 contributor を持つ tight facility を逆順 prune する。client は
選択 facility の最近点へ割り当てる。

```cpp
struct PrimalDualFacilityLocationResult {
    FacilityLocationResult solution;
    vector<long double> client_dual;
    vector<size_t> tight_facilities;
};
```

`solution.facilities` と `solution.labels` が primal 解、`client_dual` が dual
解、`tight_facilities` が tight になった順序である。member 参照は $O(1)$。

facility 数を $F$、client 数を $C$ とすると、piecewise-linear な次 event を
breakpoint sort で解くため時間計算量は $O(F^2C\log C+FC)$、空間計算量は
$O(FC)$。

## 注意点

opening cost と distance は非負有限値で、facility は1個以上必要である。
行列は長方形で opening cost 数と行数が一致しなければならない。metric distance
で標準 primal-dual 近似の前提を満たすが、非 metric 入力では近似比を保証しない。
浮動小数点 tight 判定には machine epsilon に比例した許容幅を使う。
