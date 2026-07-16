---
title: Maximum Taxi Pickups (タクシー最大送迎人数) [TAXI]
documentation_of: ../src/algorithm/matching/maximum_taxi_pickups.hpp
---

格子状道路上の乗客とタクシーについて、制限時間内に迎えに行ける一対一対応の最大数を返す。

## maximum_taxi_pickups

```cpp
int maximum_taxi_pickups(
    const vector<pair<int, int>>& passenger_positions,
    const vector<pair<int, int>>& taxi_positions,
    long long speed_meters_per_second,
    long long pickup_time_seconds,
    long long block_length_meters = 200
);
```

各座標を格子交差点として扱う。タクシーが `pickup_time_seconds` 秒以内にManhattan距離を走って到達できる乗客だけを対応可能とし、送迎できる人数の最大値を返す。

## API別の時間計算量・空間計算量

$P=passenger_positions.size()$、$T=taxi_positions.size()$、対応可能な組数を $E$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_taxi_pickups` | $O(PT+E\sqrt{P+T})$ | $O(P+T+E)$ |

## 注意点

- 速度と時間は非負、1区画の長さは正でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 距離の比較には128-bit整数を用いる。
