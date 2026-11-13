---
title: Primal-Dual Facility Location (主双対施設配置)
documentation_of: ../../../src/approximate/facility/primal_dual_facility_location.hpp
---

容量制約のない施設配置問題に対する、Jain–Vazirani 型の主双対ヒューリスティックである。`approximate::facility` 名前空間に属する。

## API

```cpp
primal_dual_facility_location(
    facility_count, client_count, opening_cost, distance);

primal_dual_facility_location(distance_matrix, opening_cost);
```

前者の `distance(facility, client)` は非負の有限値を返す。活性な顧客の双対変数を同じ速度で増やし、顧客からの寄与の和が開設費用に達した施設を tight にする。tight な施設に寄与している顧客は停止し、全ての顧客が停止した後、共通の寄与者を持つ tight な施設を逆順に刈り込む。各顧客は、選ばれた施設のうち最も近いものに割り当てる。

```cpp
struct PrimalDualFacilityLocationResult {
    FacilityLocationResult solution;
    vector<long double> client_dual;
    vector<size_t> tight_facilities;
};
```

`solution.facilities` と `solution.labels` が主問題の解、`client_dual` が双対問題の解、`tight_facilities` が tight になった順の施設列である。各メンバの参照は $O(1)$ である。

施設数を $F$、顧客数を $C$ とすると、区分線形な次のイベントを折れ点のソートで求めるため、時間計算量は $O(F^2C\log C+FC)$、空間計算量は $O(FC)$ である。

## 注意点

- 開設費用と距離は非負の有限値で、施設は 1 個以上必要である。行列は長方形で、開設費用の個数と行数が一致しなければならない。違反時は `std::invalid_argument` を送出する。
- 距離が距離空間の公理を満たせば標準的な主双対近似の前提を満たすが、そうでない入力では近似比を保証しない。
- 浮動小数点での tight の判定には、machine epsilon に比例した許容幅を使う。
