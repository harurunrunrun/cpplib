---
title: Priority Search Tree (優先探索木)
documentation_of: ../../../../src/structure/range_query/value_query/priority_search_tree.hpp
---

$x$ の範囲と $y$ の上限を指定する 3 辺領域の点を列挙するための、静的な優先探索木である。

## API

```cpp
template<class Coordinate, class Payload = int>
class PrioritySearchTree;
```

- `Point{x, y, payload}`: 1 点の座標と、列挙時に返す値を保持する。
- `PrioritySearchTree()`: 空の木を構築する。
- `explicit PrioritySearchTree(vector<Point> points)`: `points` の全要素から木を構築する。
- `size()`: 格納した点の個数を返す。
- `report(lower_x, upper_x, upper_y)`: `lower_x <= x < upper_x` かつ `y < upper_y` を満たす全点の `payload` を返す。

## 時間計算量

$N$ を点の個数、$K$ を列挙する点の個数とする。

- デフォルト構築: $O(1)$ 時間、$O(1)$ 空間
- `PrioritySearchTree(points)`: $O(N\log N)$ 時間、$O(N)$ 空間
- `report`: $O(\log N+K)$ 時間。戻り値の `vector` を除いて $O(\log N)$ の再帰スタックを使う
- `size`: $O(1)$ 時間

構築では $x$ 座標による整列を 1 回だけ行い、区間内で $y$ が最小の点の取得と削除をセグメント木で各点 1 回ずつ処理する。

## 注意点

- $x$ 座標と $y$ 座標の比較には `Coordinate::operator<` を使う。同じ $x$ 座標を持つ点や、同じ座標の複数の点も別々に格納して列挙する。
- `Payload` はコピー可能でなければならない。出力の順序は保証しない。
- 範囲は $x$、$y$ とも上端を含まない。`lower_x == upper_x` なら空の `vector` を返し、`upper_x < lower_x` なら `std::invalid_argument` を送出する。
- 内部の頂点番号に `int` を使うので、点の個数が `INT_MAX` を超える場合は `std::length_error` を送出する。
