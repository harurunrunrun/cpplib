---
title: GCD Extreme (GCDペア総和) [GCDEX]
documentation_of: ../src/algorithm/math/gcd_extreme.hpp
---

複数の上限について、異なる正整数ペアの GCD 総和を一括計算する。

# `gcd_extreme`

```cpp
vector<uint64_t> gcd_extreme(const vector<int>& queries);
```

各 `n = queries[i]` に対する `sum(gcd(x,y), 1 <= x < y <= n)` を入力順に返す。空の入力には空の配列を返す。

## API別の時間計算量・空間計算量

`N = max(queries)` とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `gcd_extreme` | $O(N\log N + |queries|)$ | $O(N)$ |

## 注意点

- 負のクエリでは `invalid_argument` を送出する。
