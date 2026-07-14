---
title: LCM Sum Table (LCM総和表) [LCMSUM]
documentation_of: ../src/algorithm/math/lcm_sum.hpp
---

`sum(lcm(i,n), 1 <= i <= n)` を繰り返し問い合わせるための前計算表。

## API

### `LcmSumTable(limit)`

```cpp
explicit LcmSumTable(int limit);
```

`1` 以上 `limit` 以下の問い合わせを処理できる表を構築する。

### `limit()`

```cpp
int limit() const;
```

構築時の上限を返す。

### `value(n)`

```cpp
uint64_t value(int n) const;
```

`sum(lcm(i,n), 1 <= i <= n)` を返す。

## 引数・戻り値

- `limit`: 構築する表の上限。
- `n`: 問い合わせる正整数。
- `limit()` は構築時の `limit` を返す。
- `value(n)` は $\sum_{i=1}^{n}\operatorname{lcm}(i,n)$ を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(limit\log\log limit)$ | $O(limit)$ |
| `limit` | $O(1)$ | $O(1)$ |
| `value` | $O(\log n)$ | $O(1)$ |

## 注意点

- `limit >= 1` を前提とする。
- `n < 1` または `n > limit()` なら `out_of_range` を送出する。
- `value(n)` の答えは `uint64_t` に収まる必要がある。
