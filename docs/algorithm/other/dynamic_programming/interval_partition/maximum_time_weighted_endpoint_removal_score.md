---
title: Maximum Time-weighted Endpoint Removal Score (時刻重み付き端点除去の最大得点) [TRT]
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/maximum_time_weighted_endpoint_removal_score.hpp
---

列の左端または右端を順に取り除くときの、時刻で重み付けした得点の最大値を区間 DP で求める。

## `maximum_time_weighted_endpoint_removal_score`

```cpp
long long maximum_time_weighted_endpoint_removal_score(
    const std::vector<long long>& values
);
```

長さ $N$ の列から、各時刻 $t=1,2,\ldots,N$ に現在の左端または右端を 1 個取り除く。時刻 $t$ に値 $x$ を取り除いたときの得点を $tx$ として、得点の総和の最大値を返す。値は負でもよい。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_time_weighted_endpoint_removal_score` | $O(N^2)$ | $O(N)$ |

## 注意点

- 空の列では $0$ を返す。
- 中間値または答えが `long long` に収まらない場合、および要素数が `int` の範囲を超える場合は `std::runtime_error` を送出する。
