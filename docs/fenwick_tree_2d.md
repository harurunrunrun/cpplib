---
title: Fenwick Tree 2D (二次元フェンウィック木)
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

- `FenwickTree2D(int, int)`: $O(\mathtt{MAX\_H}\mathtt{MAX\_W})$
- `FenwickTree2D(vector)`: $O(\mathtt{MAX\_H}\mathtt{MAX\_W}+HW\log H\log W)$
- `add`, `prefix_sum`, `sum`, `get`, `set`: $O(\log H \log W)$
- `height`, `width`, `empty`: $O(1)$

## 前提・例外・容量

- `0 <= h <= MAX_H`, `0 <= w <= MAX_W`。範囲外または非矩形vectorは例外。
- 点indexは`[0,h) x [0,w)`、prefix端点は`[0,h] x [0,w]`。
- 矩形は`0 <= y1 <= y2 <= h`, `0 <= x1 <= x2 <= w`。違反時は例外。
- `T`はdefault構築、加算、減算を使える必要がある。
- 保存領域は固定$O(MAX\_H MAX\_W)$、各操作の追加領域は$O(1)$。
