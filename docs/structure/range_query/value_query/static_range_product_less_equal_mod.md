---
title: Static Range Product Less Equal Mod (静的区間上限制約付き積) [ADATAXES]
documentation_of: ../../../../src/structure/range_query/value_query/static_range_product_less_equal_mod.hpp
---

静的な整数列について、指定した区間内で上限以下の要素だけの積を、法 `MOD` で求める。内部では `MonoidMergeSortTree` を使う。

## StaticRangeProductLessEqualMod

```cpp
template<int MOD, int MAX_SIZE = -1>
class StaticRangeProductLessEqualMod {
public:
    explicit StaticRangeProductLessEqualMod(const vector<int>& values);
    int size() const;
    int product_less_equal(int left, int right, int upper) const;
};
```

`MAX_SIZE` は列の長さの上限であり、省略時（`-1`）は上限を設けず、実行時の長さに合わせて `std::vector` で領域を確保する。

## コンストラクタ

静的な列から構造を構築する。

## size

列の長さを返す。

## product_less_equal

半開区間 `[left, right)` の要素のうち、`upper` 以下のものすべての積を `MOD` で割った余りを返す。該当する要素がない場合は乗法の単位元（`1 % MOD`）を返す。

## API 別の時間計算量・空間計算量

列の長さを $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N\log N)$ | $O(N\log N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `product_less_equal(l, r, x)` | $O(\log^2 N)$ | $O(\log N)$ |

## 注意点

- `MOD` は正でなければならない（`static_assert` で検査する）。
- 列の長さは `MAX_SIZE` 以下、区間は `0 <= left <= right <= size()` でなければならない。
- 実行時の前提違反では `std::runtime_error` を送出する。
