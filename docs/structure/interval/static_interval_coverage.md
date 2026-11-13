---
title: Static Interval Coverage (静的区間被覆数) [ADARAIN]
documentation_of: ../../../src/structure/interval/static_interval_coverage.hpp
---

整数座標上の閉区間の集合から、各座標を含む区間の個数を差分配列で前計算し、一点の被覆数を返す。

## StaticIntervalCoverage

```cpp
template<int MAX_COORDINATE = -1>
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

`MAX_COORDINATE` は座標数の上限である。省略（`-1`）すると上限を設けない。いずれの場合も内部の領域は `std::vector` で実行時の大きさに合わせて確保する。

```cpp
using Dynamic = StaticIntervalCoverage<>;
```

## コンストラクタ

座標 $[0, \texttt{coordinate_count})$ と閉区間の集合から、被覆数を構築する。

## size

座標数を返す。

## coverage_at

指定した座標を含む閉区間の個数を返す。

## 時間計算量

座標数を $N$、区間数を $M$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N+M)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `coverage_at(position)` | $O(1)$ | $O(1)$ |

## 空間計算量

$O(N)$。

## 注意点

- 各区間 $[\mathrm{left}, \mathrm{right}]$ は $0 \le \mathrm{left} \le \mathrm{right} \lt \texttt{coordinate_count}$ を満たす必要がある。
- `coordinate_count` は $0$ 以上 `MAX_COORDINATE` 以下でなければならない。
- `coverage_at` の座標は $[0, \mathrm{size}())$ でなければならない。
- 前提に違反すると `std::runtime_error` を送出する。
