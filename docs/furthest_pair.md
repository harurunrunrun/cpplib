---
title: Furthest Pair of Points (最遠点対) [furthest_pair]
documentation_of: ../src/algorithm/geometry/2d/query/furthest_pair.hpp
---

整数座標点集合の最遠点対を求める。
重複座標をまとめて凸包を構築し、rotating calipersで反対側の頂点を走査する。
返す添字は入力配列に対するものである。

## `furthest_pair`

```cpp
FurthestPairResult result = furthest_pair(points);
```

`points`は`std::vector<std::pair<Coordinate, Coordinate>>`で、`Coordinate`は整数型とする。
距離が最大となる組が複数あれば、$(\min(i,j),\max(i,j))$が辞書順最小の組を返す。
座標が重複する場合も異なる入力添字を区別する。

0点または1点では組が存在しない。2点以上がすべて同一点なら`(0, 1)`を返す。
64-bit以下の整数座標を対象とし、外積と距離平方は `ExactInteger` で
正確に計算する。

## `FurthestPairResult`

```cpp
struct FurthestPairResult {
    std::size_t first;
    std::size_t second;
    ExactInteger squared_distance;
    bool exists() const;
};
```

`first`と`second`は昇順である。`exists()`がfalseなら両添字は
`std::numeric_limits<std::size_t>::max()`である。

## API別の時間計算量・空間計算量

$N$を点数とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `furthest_pair` | $O(N\log N)$ | $O(N)$ |
| `FurthestPairResult::exists` | $O(1)$ | $O(1)$ |
| 各memberの参照 | $O(1)$ | $O(1)$ |

## 注意点

`Coordinate`は符号付き・符号なしのどちらでもよいが、幅は64-bit以下とする。
`squared_distance`は任意精度整数 `ExactInteger` で、値は常に非負である。
64-bit座標の二次元平方距離の最大値もoverflowせず保持する。
