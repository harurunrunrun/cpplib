---
title: Static Interval Coverage (静的区間被覆数) [ADARAIN]
documentation_of: ../src/structure/other/static_interval_coverage.hpp
---

整数座標上の閉区間集合を構築し、各座標を含む区間数を返す。

## StaticIntervalCoverage

```cpp
template<int MAX_COORDINATE = 1000000>
class StaticIntervalCoverage {
public:
    StaticIntervalCoverage(
        int coordinate_count,
        const vector<pair<int, int>>& closed_intervals
    );
    int size() const;
    int coverage_at(int position) const;
};
```

## Constructor

座標 $[0,coordinate\_count)$ と閉区間集合から被覆数を構築する。

## size

座標数を返す。

## coverage_at

指定座標を含む閉区間の個数を返す。

## API別の時間計算量・空間計算量

座標数を $N$、区間数を $M$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N+M)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `coverage_at(position)` | $O(1)$ | $O(1)$ |

## 注意点

- 各区間は `0 <= left <= right < coordinate_count` を満たす必要がある。
- `coordinate_count` は `MAX_COORDINATE` 以下でなければならない。
- 前提違反時は `runtime_error` を送出する。
