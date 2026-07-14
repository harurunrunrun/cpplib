---
title: Range Add Range Sum (区間加算・区間和) [CDC12_H] [HORRIBLE]
documentation_of: ../src/structure/segtree/range_add_range_sum.hpp
---

零初期化された列への区間加算と区間和を管理する。

# RangeAddRangeSum

```cpp
template<class T, int MAX_SIZE>
class RangeAddRangeSum {
public:
    explicit RangeAddRangeSum(int size);
    int size() const;
    void add(int left, int right, T value);
    T sum(int left, int right) const;
};
```

## Constructor

長さ `size` の零列を構築する。

## size

列長を返す。

## add

半開区間 `[left,right)` の各要素に `value` を加える。

## sum

半開区間 `[left,right)` の総和を返す。

## API別の時間計算量・空間計算量

列長を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `add(l,r,x)` | $O(\log N)$ | $O(1)$ |
| `sum(l,r)` | $O(\log N)$ | $O(1)$ |

## 注意点

- `size` は `MAX_SIZE` 以下、区間は `0 <= left <= right <= size()` でなければならない。
- 加算および総和は `T` で表現できる必要がある。
- 前提違反時は `runtime_error` を送出する。
