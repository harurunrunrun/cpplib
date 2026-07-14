---
title: Maximum Party Fun (予算内の最大楽しさ) [PARTY]
documentation_of: ../src/algorithm/other/maximum_party_fun.hpp
---

予算内で得られる最大の楽しさと、その達成に必要な最小費用を返す。

# PartyOption / PartyPlanValue

```cpp
struct PartyOption { int fee, fun; };
struct PartyPlanValue { int minimum_fee, maximum_fun; };
```

# maximum_party_fun

```cpp
PartyPlanValue maximum_party_fun(int budget, const vector<PartyOption>& parties);
```

各候補は高々1回選べる。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_party_fun` | $O(NB)$ | $O(B)$ |

## 注意点

- 予算・費用・楽しさは非負でなければならない。
