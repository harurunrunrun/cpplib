---
title: Range Linear Add Minimum (添字一次式加算と区間最小値) [range_linear_add_range_min]
documentation_of: ../../../../src/structure/range_query/extrema/range_linear_add_min.hpp
---

長さ $N$ の整数列に対して、区間に添字の一次式を加える更新と、区間最小値のクエリを処理する。

## API

```cpp
explicit RangeLinearAddMin(const vector<int64_t>& values)
int size() const
void add(int l, int r, int64_t b, int64_t c)
int64_t minimum(int l, int r) const
pair<int64_t, int> minimum_with_count(int l, int r) const
```

- コンストラクタは配列を約 $\sqrt N$ 個のブロックに分ける。
- `add(l, r, b, c)` は `[l, r)` の各 `i` について、`a[i]` に `b * i + c` を加える。
- `minimum(l, r)` は空でない半開区間 `[l, r)` の最小値を返す。
- `minimum_with_count(l, r)` は `(最小値, その出現回数)` を返す。
- `size()` は配列の長さを返す。

ブロックごとに下側凸包と一次式の遅延作用を持つので、ブロック全体を覆う更新では要素を走査しない。

## 時間計算量

- 構築: $O(N)$
- `add`: $O(\sqrt N)$
- `minimum`, `minimum_with_count`: $O(\sqrt N\log(N+1))$

空間計算量は $O(N)$。凸包の辺上にある点も残し、最小値を共有する区間（plateau）は二分探索で数える。

## 注意点

- 区間は `0 <= l <= r <= N` を満たす必要があり、範囲外なら `std::out_of_range` を送出する。最小値のクエリで空区間を渡すと `std::invalid_argument` を送出する。
- 値、遅延係数、`b * i + c`、および更新後のすべての値は `int64_t` に収まる必要がある。凸包の比較では `__int128` を使う。
