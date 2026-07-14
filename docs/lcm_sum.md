---
title: LCM Sum Table (LCM総和表) [LCMSUM]
documentation_of: ../src/algorithm/math/lcm_sum.hpp
---

`sum(lcm(i,n), 1 <= i <= n)` を繰り返し問い合わせるための前計算表。

# `LcmSumTable`

```cpp
explicit LcmSumTable(int limit);
```

`1..limit` の最小素因数を前計算する。

# `limit`

```cpp
int limit() const;
```

構築時の上限を返す。

# `value`

```cpp
uint64_t value(int n) const;
```

`sum(lcm(i,n), 1 <= i <= n)` を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(limit\log\log limit)$ | $O(limit)$ |
| `limit` | $O(1)$ | $O(1)$ |
| `value` | $O(\log n)$ | $O(1)$ |

## 注意点

- `limit >= 1` が必要である。
- `n` が表の外なら `out_of_range` を送出する。
