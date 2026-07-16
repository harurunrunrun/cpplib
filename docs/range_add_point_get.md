---
title: Range Add Point Get (区間加算・一点取得) [UPDATEIT]
documentation_of: ../src/structure/other/range_add_point_get.hpp
---

零初期化された列への区間加算と一点取得を行う。

## RangeAddPointGet

```cpp
template<class T, int MAX_SIZE>
class RangeAddPointGet {
public:
    explicit RangeAddPointGet(int size);
    int size() const;
    void add(int left, int right, const T& value);
    T get(int position) const;
};
```

## Constructor

長さ `size` の零列を構築する。

## size

列長を返す。

## add

半開区間 `[left,right)` の各要素に `value` を加える。

## get

指定位置の現在値を返す。

## API別の時間計算量・空間計算量

列長を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N)$ | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `add(l,r,x)` | $O(\log N)$ | $O(1)$ |
| `get(position)` | $O(\log N)$ | $O(1)$ |

## 注意点

- 列長は `MAX_SIZE` 以下でなければならない。
- 区間と位置は0-indexedで、有効範囲内でなければならない。
- `T` は加算と単項マイナスを提供する必要がある。
- 前提違反時は `runtime_error` を送出する。
