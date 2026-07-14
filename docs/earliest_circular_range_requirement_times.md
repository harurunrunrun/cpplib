---
title: Earliest Circular Range Requirement Times (巡回区間加算の必要量初達時刻) [METEORS]
documentation_of: ../src/algorithm/other/earliest_circular_range_requirement_times.hpp
---

所有位置ごとの累積量が必要量へ初めて達する操作時刻を一括計算します。

## API

### `CircularRangeAddition`

`left`、`right`、`amount` を持ちます。`left <= right` なら閉区間 `[left,right]`、それ以外なら末尾から先頭へ巡回する閉区間へ加算します。

### `earliest_circular_range_requirement_times(...)`

```cpp
vector<int> earliest_circular_range_requirement_times(
    int position_count,
    const vector<vector<int>>& owned_positions,
    const vector<long long>& requirements,
    const vector<CircularRangeAddition>& operations
);
```

所有者ごとに、必要量へ初めて達する適用済み操作数を返します。初期状態で達していれば $0$、最後まで未達なら `operations.size() + 1` です。

## 時間計算量

位置数を $P$、所有者数を $C$、操作数を $K$ として、$O((P+K)\log P\log(K+2))$ です。

## 空間計算量

$O(P+C+K)$ です。

## 注意点

- `owned_positions.size() == requirements.size()` が必要です。
- 所有位置と操作端点は `[0, position_count)` に属さなければなりません。
- 各操作の `amount` は非負でなければなりません。
- 必要量・累積量は `long long` で表現できなければなりません。
- 入力違反では `runtime_error` を送出します。
