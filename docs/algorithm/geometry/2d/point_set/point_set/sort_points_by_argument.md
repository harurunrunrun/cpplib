---
title: Sort Points by Argument (偏角ソート) [sort_points_by_argument]
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set/sort_points_by_argument.hpp
---

2 次元ベクトルを偏角順にソートする。三角関数を使わず、どちらの半平面にあるかと外積だけで比較する。

## `sort_points_by_argument`

```cpp
std::vector<Point> sorted = sort_points_by_argument(points);
```

入力の `std::vector<Point>` を値渡しで受け取り、負の $x$ 軸の直後から反時計回り、すなわち偏角 $(-\pi, \pi]$ の昇順に並べて返す。負の $x$ 軸上の点は最後に来て、原点は偏角 $0$ として扱う。
偏角が等しい点同士は $(x, y)$ の辞書順で並べるため、結果は入力の順序に依存しない。座標が完全に一致する点同士は比較上同値である。

比較には座標の `<`, `==`、乗算、減算を直接使い、許容誤差は用いない。整数座標を `Point` の `long double` に変換して使う場合は、途中の外積が正確に表現できる範囲で使用すること。

## API別の時間計算量・空間計算量

$N$ を `points.size()` とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `sort_points_by_argument(points)` | $O(N \log N)$ | $O(\log N)$ |

表の追加領域とは別に、戻り値自体が $O(N)$ の領域を持つ。

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。

## 内部API

- `sort_points_by_argument_detail::sector(point)`: 偏角順の比較に用いる区分番号を返す（$y \lt 0$ なら $-1$、正の $x$ 軸上または原点なら $0$、それ以外なら $1$）。時間計算量・空間計算量は $O(1)$。
- `sort_points_by_argument_detail::Less`: 上記の順序を表す比較関数オブジェクト。

いずれもソート関数の実装用であり、区分番号の互換性は保証しない。
