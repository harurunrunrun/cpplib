---
title: LCM Sum Table (LCM総和表) [LCMSUM]
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/lcm_sum.hpp
---

$\sum_{i=1}^{n} \operatorname{lcm}(i, n)$ を繰り返し求めるための前計算表。最小素因数の表を持ち、$\frac{n}{2}\left(1 + \sum_{d \mid n} d \, \varphi(d)\right)$ を素因数分解から計算する。

## API

### コンストラクタ `math::LcmSumTable(limit)`

```cpp
explicit LcmSumTable(int limit);
```

$1$ 以上 `limit` 以下の $n$ に対するクエリを処理できる表（最小素因数の篩）を構築する。

### `limit()`

```cpp
int limit() const;
```

構築時の上限を返す。

### `value(n)`

```cpp
std::uint64_t value(int n) const;
```

$\sum_{i=1}^{n} \operatorname{lcm}(i, n)$ を返す。

## 時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(L \log \log L)$（$L$ は `limit`） | $O(L)$ |
| `limit` | $O(1)$ | $O(1)$ |
| `value` | $O(\log n)$ | $O(1)$ |

## 注意点

- `limit >= 1` が必要である。`limit == 0` では `std::invalid_argument` を送出する（負の値は表の確保の時点で失敗する）。
- `n < 1` または `n > limit()` なら `std::out_of_range` を送出する。
- `value(n)` の答えは `uint64_t` に収まる必要がある（中間値は `unsigned __int128` で計算する）。
