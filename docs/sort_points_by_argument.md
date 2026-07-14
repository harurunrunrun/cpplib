---
title: Sort Points by Argument (偏角ソート) [sort_points_by_argument]
documentation_of: ../src/algorithm/geometry/2d/sort_points_by_argument.hpp
---

2次元vectorを偏角順にsortする。三角関数を使わず、半平面と外積だけを比較する。

## `sort_points_by_argument`

```cpp
std::vector<Point> sorted = sort_points_by_argument(points);
```

入力vectorを値渡しで受け取り、負の$x$軸の直後から反時計回り、すなわち
偏角$[-\pi,\pi]$の昇順で並べて返す。負の$x$軸は最後、原点は偏角0として扱う。
同じ偏角では`x`, `y`の辞書順をtie-breakに使うため、返り値は入力順に依存しない。
同一点は比較上同値である。

比較は座標の`<`, `==`, 乗算、減算を直接使う。整数を`Point`の
`long double`へ変換する場合、計算途中の外積を正確に表現できる範囲で使用する。

## API別の時間計算量・空間計算量

`N = points.size()`とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `sort_points_by_argument(points)` | $O(N\log N)$ | $O(\log N)$ |

表の追加領域とは別に、返り値自体が$O(N)$の領域を持つ。

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。

## 内部API

- `sort_points_by_argument_internal::sector(point)`: 偏角順比較用の半平面番号を返す。時間計算量・空間計算量は $O(1)$。

上位sort関数の実装用であり、内部sector番号の互換性は保証しない。
