---
title: Convex Hull Trick (凸包トリック)
documentation_of: ../../../../src/structure/convex_hull_trick/line_container/convex_hull_trick.hpp
---

直線の追加と、ある $x$ における最小値の取得を行う。直線は傾きが単調非増加になる順に追加する。

## テンプレート引数

```cpp
ConvexHullTrick<MAX_LINE = -1>
```

- `MAX_LINE`: 保持する直線数の上限。省略（`-1`）すると内部で `std::vector` を使う可変長版になり、必要に応じて領域を伸長する。

```cpp
using Variable = ConvexHullTrick<>;
using Fixed = ConvexHullTrick<100000>;
```

## メンバ関数

### add_line

```cpp
void add_line(long long a, long long b)
```

直線 $y = ax + b$ を追加する。直前に追加した直線と傾きが等しい場合は切片の小さい方だけを残す。

### query

```cpp
long long query(long long x) const
```

追加された直線の $x$ における最小値を返す。

### query_monotone_inc

```cpp
long long query_monotone_inc(long long x)
```

$x$ が単調非減少に与えられる場合の最小値クエリ。内部のポインタを前進させながら答える。

### size, empty

```cpp
int size() const
bool empty() const
```

凸包上に残っている直線数と、それが $0$ かどうかを返す。

### Line

```cpp
struct Line{ long long a, b; };
__int128 Line::eval128(long long x) const
long long Line::eval(long long x) const
```

`eval128` は $ax + b$ を `__int128` で返し、`eval` はそれを `long long` の範囲に丸めて返す。

## 時間計算量

$N$ を現在の凸包上の直線数とする。

- デフォルトコンストラクタ、`size`、`empty`、`Line::eval128`、`Line::eval`: $O(1)$
- `add_line`: 1 回では最悪 $O(N)$。各直線は高々 1 回しか除去されないため、追加列全体で償却 $O(1)$
- `query`: $O(\log(N+1))$
- `query_monotone_inc`: 1 回では最悪 $O(N)$。$x$ が単調非減少なクエリ列ではポインタが前進するだけなので償却 $O(1)$

可変長版でも領域の伸長コストは償却され、上記の計算量を保つ。

## 空間計算量

- 固定長版: $O(\texttt{MAX_LINE})$
- 可変長版: 保持している直線数に比例する
- 各操作の追加領域: $O(1)$

## 注意点

- `add_line` の傾きは追加順に単調非増加でなければならない。違反すると `std::runtime_error` を送出する。
- 固定長版で、有効な直線数が `MAX_LINE` を超える追加は `std::runtime_error` を送出する。同じ傾きで劣る直線は保存しないので容量を消費しない。
- 可変長版で、`int` で表せる上限を超える要素数は例外となる。
- 空の状態での `query` と `query_monotone_inc` は `std::runtime_error` を送出する。
- `query_monotone_inc` に渡す $x$ はクエリ列全体で単調非減少でなければならない。実装は検査しない。
- 比較は `__int128` で行い、戻り値だけを `long long` の範囲に丸める。
