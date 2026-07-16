---
title: Maximum Time-weighted Endpoint Removal Score (時刻重み付き端点除去の最大得点) [TRT]
documentation_of: ../src/algorithm/other/maximum_time_weighted_endpoint_removal_score.hpp
---

列の左端または右端を順に取り除くとき、時刻重み付き得点の最大値を求める。

## `maximum_time_weighted_endpoint_removal_score`

```cpp
long long maximum_time_weighted_endpoint_removal_score(
    const vector<long long>& values
);
```

長さ $N$ の列から、各時刻 $t=1,2,\ldots,N$ に現在の左端または右端を
1個取り除く。時刻 $t$ に値 $x$ を取り除いた得点を $tx$ としたときの、
全得点の最大値を返す。値は負でもよい。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_time_weighted_endpoint_removal_score` | $O(N^2)$ | $O(N)$ |

## 注意点

- 空列では0を返す。
- 中間値または答えが `long long` に収まらない場合は `runtime_error` を送出する。
