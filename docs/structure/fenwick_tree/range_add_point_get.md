---
title: Range Add Point Get (区間加算・一点取得) [UPDATEIT]
documentation_of: ../../../src/structure/fenwick_tree/range_add_point_get.hpp
---

零初期化された列への区間加算と一点取得を行う。内部では差分列を `FenwickTree` で管理する。

## RangeAddPointGet

```cpp
template<class T, int MAX_SIZE = -1>
class RangeAddPointGet {
public:
    explicit RangeAddPointGet(int size);
    int size() const;
    void add(int left, int right, const T& value);
    T get(int position) const;
};
```

`MAX_SIZE` を省略（`-1`）すると、実行時の要素数に合わせて `std::vector` を確保する可変長版になる。

```cpp
RangeAddPointGet<long long> values(n);
```

## コンストラクタ

長さ `size` の零列を構築する。

## size

列の長さを返す。

## add

半開区間 $[\mathrm{left}, \mathrm{right})$ の各要素に `value` を加える。

## get

指定した位置の現在値を返す。

## 時間計算量

列の長さを $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N)$（固定長版は $O(\texttt{MAX_SIZE})$） | $O(N)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `add(l, r, x)` | $O(\log N)$ | $O(1)$ |
| `get(position)` | $O(\log N)$ | $O(1)$ |

## 空間計算量

固定長版は $O(\texttt{MAX_SIZE})$（ヒープ上に確保する）、可変長版は $O(N)$。

## 注意点

- 列の長さは `MAX_SIZE` 以下でなければならない。
- 区間と位置は 0-indexed で、有効範囲内でなければならない。
- `T` は加算と単項マイナスを提供する必要がある。
- 前提に違反すると `std::runtime_error` を送出する。
