---
title: Static Range Minimum (静的区間最小値) [RMQSQ] [RPLN]
documentation_of: ../src/structure/other/static_range_minimum.hpp
---

静的列の半開区間最小値を返す。

## StaticRangeMinimum

```cpp
template<class T, int MAX_SIZE>
class StaticRangeMinimum {
public:
    explicit StaticRangeMinimum(const vector<T>& values);
    int size() const;
    T minimum(int left, int right) const;
};
```

## Constructor

静的列から問い合わせ構造を構築する。

## size

列長を返す。

## minimum

半開区間 `[left,right)` の最小値を返す。

## API別の時間計算量・空間計算量

列長を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N\log N)$ | $O(N\log N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `minimum(l,r)` | $O(1)$ | $O(1)$ |

## 注意点

- 列長は `MAX_SIZE` 以下でなければならない。
- 問い合わせ区間は空でない `0 <= left < right <= size()` を満たす必要がある。
- 前提違反時は `runtime_error` を送出する。
