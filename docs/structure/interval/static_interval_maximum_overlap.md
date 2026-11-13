---
title: Static Interval Maximum Overlap (静的区間最大重なり数) [BGSHOOT]
documentation_of: ../../../src/structure/interval/static_interval_maximum_overlap.hpp
---

静的な閉区間の集合について、問い合わせの閉区間内の点で重なっている区間の個数の最大値を返す。
端点で座標圧縮した各点の被覆数を Sparse Table に載せる。

## StaticIntervalMaximumOverlap

```cpp
template<class Coordinate, int MAX_ENDPOINTS = -1>
class StaticIntervalMaximumOverlap {
public:
    explicit StaticIntervalMaximumOverlap(
        vector<pair<Coordinate, Coordinate>> closed_intervals
    );
    int interval_count() const;
    int maximum_overlap(Coordinate left, Coordinate right) const;
};
```

`MAX_ENDPOINTS` は相異なる端点の個数の上限（内部の Sparse Table の容量）である。省略（`-1`）すると、実行時の個数に合わせて `std::vector` を確保する可変長版になる。

```cpp
using Dynamic = StaticIntervalMaximumOverlap<long long>;
```

## コンストラクタ

閉区間の集合から構築する。各区間の端点の順序は自動的に正規化する。

## interval_count

登録した区間の個数を返す。

## maximum_overlap

閉区間 $[\mathrm{left}, \mathrm{right}]$ 内の点のうち、登録した区間による被覆数の最大値を返す。端点の順序は自動的に正規化する。

## 時間計算量

登録した区間の個数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `interval_count()` | $O(1)$ | $O(1)$ |
| `maximum_overlap(l, r)` | $O(\log N)$ | $O(1)$ |

## 空間計算量

$O(N\log N)$（Sparse Table）。

## 注意点

- 相異なる端点の個数は `MAX_ENDPOINTS` 以下でなければならない。違反すると内部の Sparse Table が `std::runtime_error` を送出する。
- `Coordinate` は全順序を持たなければならない。
- 空の区間集合に対する問い合わせは $0$ を返す。
