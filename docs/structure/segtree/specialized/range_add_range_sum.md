---
title: Range Add Range Sum (区間加算・区間和) [CDC12_H] [HORRIBLE]
documentation_of: ../../../../src/structure/segtree/specialized/range_add_range_sum.hpp
---

0 で初期化された列に対して、区間加算と区間和を管理する。

## RangeAddRangeSum

```cpp
template<class T, int MAX_SIZE = -1>
class RangeAddRangeSum {
public:
    explicit RangeAddRangeSum(int size);
    int size() const;
    void add(int left, int right, T value);
    T sum(int left, int right) const;
};
```

内部では、ノードを必要に応じて動的に確保する `DynamicLazySegtree` を使う。`MAX_SIZE` を省略すると可変長版になり、長さの上限がなくなる。

## コンストラクタ

長さ `size` の全 0 列を構築する。

## size

列の長さを返す。

## add

半開区間 $[left, right)$ の各要素に `value` を加える。

## sum

半開区間 $[left, right)$ の総和を返す。

## API 別の時間計算量・空間計算量

列の長さを $N$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(1)$ | $O(1)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `add(l, r, x)` | $O(\log N)$ | ノード $O(\log N)$ |
| `sum(l, r)` | $O(\log N)$ | スタック $O(\log N)$ |

保存領域は生成済みのノード数に比例し、$O(\min(N, Q\log N))$ である（$Q$ は `add` の回数）。

## 注意点

- 固定容量版では `size` は `MAX_SIZE` 以下でなければならない。区間は `0 <= left <= right <= size()` でなければならない。
- 加算の結果と総和は `T` で表現できる必要がある。
- 前提違反時は `std::runtime_error` を送出する。
