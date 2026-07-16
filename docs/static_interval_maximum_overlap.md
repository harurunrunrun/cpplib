---
title: Static Interval Maximum Overlap (静的区間最大重なり数) [BGSHOOT]
documentation_of: ../src/structure/other/static_interval_maximum_overlap.hpp
---

静的な閉区間集合について、問い合わせ閉区間内の点で重なる区間数の最大値を返す。

## StaticIntervalMaximumOverlap

```cpp
template<class Coordinate, int MAX_ENDPOINTS>
class StaticIntervalMaximumOverlap {
public:
    explicit StaticIntervalMaximumOverlap(
        vector<pair<Coordinate, Coordinate>> closed_intervals
    );
    int interval_count() const;
    int maximum_overlap(Coordinate left, Coordinate right) const;
};
```

## Constructor

閉区間集合を構築する。端点の順序は自動的に正規化される。

## interval_count

登録区間数を返す。

## maximum_overlap

閉区間 `[left,right]` 内で、登録区間の被覆数が最大となる値を返す。端点の順序は自動的に正規化される。

## API別の時間計算量・空間計算量

登録区間数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N)$ | $O(N\log N)$ |
| `interval_count()` | $O(1)$ | $O(1)$ |
| `maximum_overlap(l,r)` | $O(\log N)$ | $O(1)$ |

## 注意点

- 相異なる端点数は `MAX_ENDPOINTS` 以下でなければならない。
- `Coordinate` は全順序を持たなければならない。
- 空の区間集合に対する問い合わせは0を返す。
- 容量違反時は下位APIから `runtime_error` が送出される。
