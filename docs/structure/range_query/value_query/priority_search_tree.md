---
title: Priority Search Tree (優先探索木)
documentation_of: ../../../../src/structure/range_query/value_query/priority_search_tree.hpp
---

$x$ の範囲と $y$ の上限を指定する三辺領域列挙用の静的優先探索木。

## API

- `Point{x,y,payload}`: 1点の座標と返却用の値を保持する。
- `PrioritySearchTree()`: 空の木を構築する。
- `PrioritySearchTree(points)`: `points` の全要素から木を構築する。
- `size()`: 格納した点数を返す。
- `report(lower_x,upper_x,upper_y)`:
  `lower_x <= x < upper_x` かつ `y < upper_y` の全点の `payload` を返す。

## 時間計算量

- デフォルト構築: $O(1)$ 時間、$O(1)$ 空間。
- `PrioritySearchTree(points)`: $O(N\log N)$ 時間、$O(N)$ 空間。
- `report`: $O(\log N+K)$ 時間、返却vectorを除いて
  $O(\log N)$ 再帰スタック。
- `size`: $O(1)$ 時間。

ここで $N$ は点数、$K$ は列挙点数である。構築では x 座標による整列を一度だけ
行い、区間最小 y の取得と削除をセグメント木で各点1回ずつ処理する。

## 注意点

- x 座標と y 座標の比較には `Coordinate::operator<` を用いる。同じ x 座標を持つ
  点や、同一座標の複数点も別々に格納して列挙する。
- `Payload` はコピー可能でなければならない。出力順は保証しない。
- 範囲は x、y とも上端を含まない。`lower_x == upper_x` なら空vectorを返す。
  `upper_x < lower_x` なら `std::invalid_argument` を送出する。
- 内部の頂点番号に `int` を使うため、点数が `INT_MAX` を超える場合は
  `std::length_error` を送出する。
