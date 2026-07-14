---
title: Angle Interval Set (円周角区間集合)
documentation_of: ../src/algorithm/geometry/2d/angle_interval_set.hpp
---

角度を2π周期で管理し、円周上の半開区間の和集合を保持する。

# 構築

~~~cpp
AngleIntervalSet()
~~~

空の集合を構築する。

# 更新

~~~cpp
void add(long double start, long double end)
void remove(long double start, long double end)
void clear()
~~~

addはstartからendへ反時計回りに進む半開区間を追加する。removeは同じ規約の区間を削除する。clearは全区間を削除する。

# 問い合わせ

~~~cpp
bool contains(long double angle) const
bool covers(long double start, long double end) const
long double covered_length() const
size_t interval_count() const
bool empty() const
bool full() const
vector<pair<long double, long double>> intervals() const
~~~

- contains: 角度が和集合に含まれるかを返す。左端を含み、右端を含まない。
- covers: 指定した反時計回り半開区間をすべて覆うかを返す。空区間は常にtrue。
- covered_length: 被覆された角度の総長を0以上2π以下で返す。
- interval_count: 内部の正規化線形区間の本数を返す。0を跨ぐ1個の円周区間は2本として数える場合がある。
- empty, full: それぞれ空集合・全周被覆かを返す。
- intervals: 0以上2π未満へ分割した、互いに交わらない半開区間を左端順で返す。

# 時間計算量

Mを更新前の線形区間数、Kを更新で併合または分割される区間数とする。

- add, remove: O((K+1) log(M+1))
- contains, covers: O(log(M+1))
- covered_length, interval_count, empty, full: O(1)
- intervals: O(M)
- clear: O(M)

# 空間計算量

保持する区間にO(M)、intervalsの戻り値にO(M)。

## 注意点

- 端点はradianで指定する。有限値でなければinvalid_argument。
- abs(end-start)が2π-GEOMETRY_EPS以上なら全周、正規化した長さがGEOMETRY_EPS以下なら空区間として扱う。
- GEOMETRY_EPS以下のgapは数値安定性のため併合する。
- 端点の包含が必要な離散問題では、半開区間規約へ変換してから使う。
