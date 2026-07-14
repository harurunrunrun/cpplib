---
title: Maximum Interval Overlap (区間最大重複数) [BYTESE2]
documentation_of: ../src/algorithm/range/maximum_interval_overlap.hpp
---

区間列をイベントへ変換して走査し、同じ点を同時に含む区間数の最大値を求める。
端点が等しいときの重複判定を、半開区間と閉区間から選べる。

# IntervalEndpointPolicy

```cpp
enum class IntervalEndpointPolicy {
    half_open,
    closed,
};
```

- `half_open`: 各入力を $[left,right)$ と扱う。$left=right$ の空区間はどの点も含まない。
- `closed`: 各入力を $[left,right]$ と扱う。$left=right$ の一点区間もその点を含む。

# maximum_interval_overlap

```cpp
template<class T>
int maximum_interval_overlap(
    const vector<pair<T, T>>& intervals,
    IntervalEndpointPolicy endpoint_policy = IntervalEndpointPolicy::half_open
)
```

指定した端点規則で、全区間が共通して含む区間数の最大値を返す。入力が空、または半開の空区間だけなら $0$ を返す。

## API別の時間計算量・空間計算量

$N=intervals.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_interval_overlap(intervals, endpoint_policy)` | $O(N\log(N+1))$ | $O(N)$ |

## 注意点

- 各区間は `left <= right` でなければならない。逆転した区間、または定義されていない `endpoint_policy` では `runtime_error` を送出する。
- `T` はコピー、`operator<` を扱える型とする。座標の減算や番兵値は使用しない。
- 同じ座標では、半開区間は終了イベントを開始イベントより先に、閉区間は開始イベントを終了イベントより先に処理する。
