---
title: Minimize Maximum Partition Sum (最大区間和最小化) [SEQPAR]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/minimize_maximum_partition_sum.hpp
---

整数列を順序を保ったままちょうど $k$ 個の空でない連続区間に分割し、区間の和の最大値を最小化する。負の数を含む列にも対応する。答えを二分探索し、各判定では「上限以下の区間で接頭辞を分割するときの区間数の最小値と最大値」を Fenwick 木で管理する。

## API

```cpp
long long minimize_maximum_partition_sum(
    const std::vector<long long>& values,
    std::size_t partition_count
)
```

## 引数・戻り値

- `values`: 分割する整数列。負の数を含んでよい。
- `partition_count`: 作る空でない連続区間の個数 $k$。
- 戻り値: 順序を保ってちょうど $k$ 個の区間に分割したときの、区間の和の最大値としてありうる最小値。分割位置は返さない。

## API別の時間計算量・空間計算量

$N$ を `values.size()`、$v_i$ を `values[i]` として $W=2+\sum_i \lvert v_i \rvert$ とする。

| API・場合 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `partition_count == 1` または `partition_count == N` | $O(N)$ | `partition_count == 1` は $O(N)$、`partition_count == N` は $O(1)$ |
| `minimize_maximum_partition_sum`（一般の場合） | $O(N\log N\log W)$ | $O(N)$ |

## 注意点

- `values` は空でなく、`1 <= partition_count <= values.size()` でなければならない。違反時は `std::invalid_argument` を送出する。
- 各区間は空でなく、全要素をちょうど 1 回ずつ含む。
- 答えが `long long` で表現できない場合は `std::overflow_error` を送出する。中間の和は `__int128` で計算する。
