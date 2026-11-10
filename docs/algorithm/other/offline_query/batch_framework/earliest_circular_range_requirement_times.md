---
title: Earliest Circular Range Requirement Times (巡回区間加算の必要量初達時刻) [METEORS]
documentation_of: ../../../../../src/algorithm/other/offline_query/batch_framework/earliest_circular_range_requirement_times.hpp
---

巡回する区間への加算操作を順に適用したとき、各所有者について、所有する位置の累積量の合計が必要量へ初めて達する時刻を並列二分探索で一括計算する。

## API

### `CircularRangeAddition`

`int left`、`int right`、`long long amount` を持つ。`left <= right` なら閉区間 $[\mathtt{left}, \mathtt{right}]$ に、それ以外なら末尾から先頭へ巡回する閉区間（$[\mathtt{left}, P)$ と $[0, \mathtt{right}]$ の和集合）に `amount` を加算する。

### `earliest_circular_range_requirement_times(...)`

```cpp
vector<int> earliest_circular_range_requirement_times(
    int position_count,
    const vector<vector<int>>& owned_positions,
    const vector<long long>& requirements,
    const vector<CircularRangeAddition>& operations
);
```

所有者 `i` ごとに、`owned_positions[i]` の各位置の累積量の合計が `requirements[i]` 以上になる最小の適用済み操作数を返す。初期状態で達していれば（`requirements[i] <= 0` なら）$0$、最後まで達しなければ `operations.size() + 1` を返す。

## 時間計算量

位置数を $P$、所有者数を $C$、所有位置の総数を $S$、操作数を $K$ として、$O((P+S+K)\log P\log(K+2))$。

## 空間計算量

$O(P+C+K)$（入力を除く）。

## 注意点

- `position_count >= 0` かつ `owned_positions.size() == requirements.size()` でなければならない。
- 所有位置と操作の端点は $[0, \texttt{position_count})$ に属さなければならない。
- 各操作の `amount` は非負でなければならない。
- 必要量・累積量は `long long` で表現できなければならない。
- 以上の入力違反では `runtime_error` を送出する。
