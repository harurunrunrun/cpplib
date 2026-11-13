---
title: Fenwick Tree 2D (二次元フェンウィック木) [MATSUM]
documentation_of: ../../../src/structure/fenwick_tree/fenwick_tree_2d.hpp
---

2 次元の一点加算と矩形和を扱う Fenwick Tree。

## 使い方

```cpp
FenwickTree2D<long long, 1000, 1000> fw(h, w);  // 固定長版
FenwickTree2D<long long> bit(h, w);             // 可変長版
fw.add(3, 4, 10);
long long s = fw.sum(1, 2, 5, 6);
```

テンプレート引数は `FenwickTree2D<T, MAX_H = -1, MAX_W = -1>`。上限を省略（`-1`）すると、実行時の大きさに合わせて `std::vector` を確保する可変長版になる。

## メンバ関数

- `explicit FenwickTree2D(int h = MAX_H, int w = MAX_W)`
    - $h \times w$ のグリッドを `T{}` で初期化する。可変長版ではデフォルトは $0 \times 0$ である。
- `explicit FenwickTree2D(const vector<vector<T>>& v)`
    - `v` で初期化する。
- `int height() const`
    - 高さを返す。
- `int width() const`
    - 幅を返す。
- `bool empty() const`
    - 高さまたは幅が $0$ かどうかを返す。
- `void add(int y, int x, T value)`
    - $a_{y,x} \gets a_{y,x} + \mathrm{value}$ とする。
- `T prefix_sum(int y, int x) const`
    - $[0, y) \times [0, x)$ の総和を返す。
- `T sum(int y1, int x1, int y2, int x2) const`
    - $[y_1, y_2) \times [x_1, x_2)$ の総和を返す。
- `T get(int y, int x) const`
    - $a_{y,x}$ を返す。
- `void set(int y, int x, T value)`
    - $a_{y,x} \gets \mathrm{value}$ とする。

## 時間計算量

$H$, $W$ を実際の高さと幅とする。

- `FenwickTree2D(int, int)`: 固定長版は $O(\texttt{MAX_H}\texttt{MAX_W})$、可変長版は $O(HW)$
- `FenwickTree2D(vector)`: 上記に加えて $O(HW\log H\log W)$
- `add`、`prefix_sum`、`sum`、`get`、`set`: $O(\log H \log W)$
- `height`、`width`、`empty`: $O(1)$

## 空間計算量

- 固定長版: $O(\texttt{MAX_H}\texttt{MAX_W})$、可変長版: $O(HW)$
- 各関数の追加領域: $O(1)$

## 注意点

- $0 \le h \le \texttt{MAX_H}$、$0 \le w \le \texttt{MAX_W}$ でなければならない。範囲外、または `vector` が矩形でない場合は `std::runtime_error` を送出する。
- 点の添字は $[0, h) \times [0, w)$、`prefix_sum` の端点は $[0, h] \times [0, w]$ でなければならない。
- 矩形は $0 \le y_1 \le y_2 \le h$、$0 \le x_1 \le x_2 \le w$ でなければならない。違反すると `std::runtime_error` を送出する。
- `T` はデフォルト構築、加算、減算ができる必要がある。
