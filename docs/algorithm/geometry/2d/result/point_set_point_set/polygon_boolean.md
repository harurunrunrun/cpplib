---
title: General Polygon Boolean Operations (一般多角形集合演算)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set_point_set/polygon_boolean.hpp
---

even-odd 規則で表した 2 つの多角形集合について、和・積・差・対称差の境界閉路を構築する。

## API

```cpp
enum class PolygonBooleanOperation{
    set_union, intersection, difference, symmetric_difference,
};

std::vector<std::vector<Point>> polygon_boolean(
    std::vector<Point> first,
    std::vector<Point> second,
    PolygonBooleanOperation operation
);
std::vector<std::vector<Point>> polygon_boolean(
    std::vector<std::vector<Point>> first,
    std::vector<std::vector<Point>> second,
    PolygonBooleanOperation operation
);
```

- `PolygonBooleanOperation` は和 `set_union`、積 `intersection`、差 `difference`（`first` から `second` を除く）、対称差 `symmetric_difference` の 4 種類である。
- 1 つ目の多重定義は単純多角形を 2 個受け取る。2 つ目は各集合をリング（閉路）の列 `std::vector<std::vector<Point>>` として受け取り、外周・穴・複数の成分を表せる。
- 戻り値は、結果集合の内部を左に見る向きの閉路の列であり、穴は外周と逆向きになる。

## 時間計算量

入力の辺の総数を $N$、分割後の境界片の数を $K = O(N^2)$ とする。全辺対の交点の生成と各辺上のイベントのソートに $O(N^2 \log N)$、各境界片の左右のサンプル点を入力の全リングと照合する分類に $O(NK)$、端点のクラスタリング・重複除去・角度順のつなぎ合わせに $O(K \log K)$ かかる。全体で時間計算量は $O(N^2 \log N + NK + K \log K)$（最悪 $O(N^3)$）、空間計算量は $O(N^2 + K)$ である。

## 注意点

- 各入力リングは自己交差しない非退化な単純多角形とする。リング間の包含関係は even-odd 規則で解釈する。
- 正規化（重複頂点の除去）後に頂点数が 3 未満のリングがある場合や、座標が有限でない場合は `std::invalid_argument` を送出する。
- 接触や同一直線上で重なる辺は分割して処理する。
- 幾何述語は有限精度の `long double` で評価する。
