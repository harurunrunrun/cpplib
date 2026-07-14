---
title: Maximum Simultaneous Guests (最大同時滞在人数) [BYTESE2]
documentation_of: ../src/algorithm/range/maximum_simultaneous_guests.hpp
---

# maximum_simultaneous_guests

```cpp
int maximum_simultaneous_guests(
    const vector<pair<int, int>>& stays
)
```

各滞在を半開区間 `[entry, exit)` としたとき、同時に滞在する人数の最大値を返す。

## 引数

- `stays`: 入場時刻と退場時刻の組。

## 戻り値

同時滞在人数の最大値。入力が空なら0。

## API別の時間計算量・空間計算量

$N=stays.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_simultaneous_guests` | $O(N\log(N+1))$ | $O(N)$ |

## 注意点

- 各組は `entry <= exit` でなければならない。違反時は `runtime_error` を送出する。
- `entry == exit` の滞在は人数に数えない。
