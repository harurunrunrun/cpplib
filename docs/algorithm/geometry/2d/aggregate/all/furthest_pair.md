---
title: Furthest Pair of Integer Points Aggregator (整数点の最遠点対集約ヘッダ) [furthest_pair]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/furthest_pair.hpp
---

整数点の最遠点対結果型と点列入力APIをまとめて読み込む後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `furthest_pair_result.hpp` | `FurthestPairResult` |
| `furthest_pair_points.hpp` | `furthest_pair(points)` |

必要なleafだけを直接includeできる。従来の `furthest_pair.hpp` も
引き続き全APIを提供する。

## 集約されるAPI

```cpp
FurthestPairResult furthest_pair(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

平方距離が最大の2点を入力添字で返す。同率なら昇順添字対が辞書順最小の組を選ぶ。

## API別の時間計算量・空間計算量

点数を $N$ とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `furthest_pair(points)` | $O(N\log N)$ | $O(N)$ |
| `FurthestPairResult::exists()`、各fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

- `Coordinate` は64-bit以下の整数型でなければならない。
- 0点または1点では `exists()` がfalseの結果を返す。
- 2点以上がすべて同じ座標なら添字対 `(0, 1)` を返す。
- 平方距離と凸包判定は `ExactInteger` で誤差なく計算する。
