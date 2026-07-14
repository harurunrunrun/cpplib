---
title: Static Range Product Less Equal Mod (静的区間上限制約付き積) [ADATAXES]
documentation_of: ../src/structure/other/static_range_product_less_equal_mod.hpp
---

静的な整数列について、指定区間内で上限以下の要素のみを法 `MOD` で乗算する。

# StaticRangeProductLessEqualMod

```cpp
template<int MOD, int MAX_SIZE>
class StaticRangeProductLessEqualMod {
public:
    explicit StaticRangeProductLessEqualMod(const vector<int>& values);
    int size() const;
    int product_less_equal(int left, int right, int upper) const;
};
```

## Constructor

静的列を構築する。

## size

列長を返す。

## product_less_equal

半開区間 `[left,right)` のうち `upper` 以下の全要素の積を `MOD` で割った余りを返す。該当要素がない場合は乗法単位元を返す。

## API別の時間計算量・空間計算量

列長を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N)$ | $O(N\log N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `product_less_equal(l,r,x)` | $O(\log^2 N)$ | $O(\log N)$ |

## 注意点

- `MOD` は正でなければならない。
- 列長は `MAX_SIZE` 以下、区間は `0 <= left <= right <= size()` でなければならない。
- 前提違反時は `runtime_error` を送出する。
