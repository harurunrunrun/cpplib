---
title: Minimize Maximum Partition Sum (最大区間和最小化) [SEQPAR]
documentation_of: ../src/algorithm/other/minimize_maximum_partition_sum.hpp
---

整数列をちょうど `k` 個の空でない連続区間へ分割し、各区間和の最大値を最小化する。負数を含む列に対応する。

## API

```cpp
long long minimize_maximum_partition_sum(
    const vector<long long>& values,
    size_t partition_count
)
```

## 引数・戻り値

- `values`: 分割する整数列。負数を含められる。
- `partition_count`: 作る非空連続区間の個数。
- 戻り値: 順序を保ってちょうど `partition_count` 区間へ分割したときの、
  区間和の最大値として実現できる最小値。分割位置は返さない。

## API別の時間計算量・空間計算量

`N = values.size()`、$W=2+\sum_i |values_i|$ とする。

| API・場合 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `partition_count == 1` または `partition_count == N` | $O(N)$ | `partition_count == 1` は $O(N)$、`partition_count == N` は $O(1)$ |
| `minimize_maximum_partition_sum`（一般の場合） | $O(N\log N\log W)$ | $O(N)$ |

## 注意点

- `values` は空でなく、`1 <= partition_count <= values.size()` でなければならない。違反時は `invalid_argument` を送出する。
- 各区間は空でなく、全要素をちょうど1回含む必要がある。
- 答えが `long long` で表現できない場合は `overflow_error` を送出する。
