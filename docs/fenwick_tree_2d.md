---
title: Fenwick Tree 2D
documentation_of: ../src/structure/other/fenwick_tree_2d.hpp
---

二次元の一点加算・矩形和。

## 使い方

```cpp
FenwickTree2D<long long, 1000, 1000> fw(h, w);
fw.add(3, 4, 10);
long long s = fw.sum(1, 2, 5, 6);
```

## メンバ関数

- `FenwickTree2D(int h = MAX_H, int w = MAX_W)`
    - `h * w` のグリッドを `0` で初期化する。
- `FenwickTree2D(vector<vector<T>> v)`
    - `v` で初期化する。
- `int height() const`
    - 高さを返す。
- `int width() const`
    - 幅を返す。
- `bool empty() const`
    - 高さまたは幅が `0` かを返す。
- `void add(int y, int x, T value)`
    - `a[y][x] += value`。
- `T prefix_sum(int y, int x) const`
    - `[0, y) * [0, x)` の総和を返す。
- `T sum(int y1, int x1, int y2, int x2) const`
    - `[y1, y2) * [x1, x2)` の総和を返す。
- `T get(int y, int x) const`
    - `a[y][x]` を返す。
- `void set(int y, int x, T value)`
    - `a[y][x] = value`。

## 計算量

- 構築: $O(HW \log H \log W)$
- `add`, `prefix_sum`, `sum`, `get`, `set`: $O(\log H \log W)$
