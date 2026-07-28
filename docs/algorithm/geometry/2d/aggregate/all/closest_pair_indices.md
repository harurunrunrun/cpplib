---
title: Closest Pair of Integer Points Aggregator (整数点の最近点対集約ヘッダ) [closest_pair]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/closest_pair_indices.hpp
---

整数点の最近点対結果型と点列入力APIをまとめて読み込む後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `closest_pair_result.hpp` | `ClosestPairResult` |
| `closest_pair_indices_points.hpp` | `closest_pair_indices(points)` |

必要なleafだけを直接includeできる。従来の `closest_pair_indices.hpp` も
引き続き全APIを提供する。

## 集約されるAPI

```cpp
ClosestPairResult closest_pair_indices(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

平方距離が最小の2点を入力添字で返す。同率なら昇順添字対が辞書順最小の組を選ぶ。

## API別の時間計算量・空間計算量

点数を $N$ とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `closest_pair_indices(points)` | $O(N\log N)$ | $O(N)$ |
| `ClosestPairResult::exists()`、各fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

- `Coordinate` は64-bit以下の整数型でなければならない。
- 0点または1点では `exists()` がfalseの結果を返す。
- 平方距離は `ExactInteger` で誤差なく保持する。
