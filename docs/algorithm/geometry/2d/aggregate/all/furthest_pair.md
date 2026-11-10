---
title: Furthest Pair of Integer Points Aggregator (整数点の最遠点対集約ヘッダ) [furthest_pair]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/furthest_pair.hpp
---

整数点の最遠点対の結果型と、点列を入力とする API をまとめて読み込む後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `furthest_pair_result.hpp` | `FurthestPairResult` |
| `furthest_pair_points.hpp` | `furthest_pair(points)` |

必要な leaf header だけを直接 include することもできる。従来どおり `furthest_pair.hpp` を include すれば全 API を使える。

## 集約される API

```cpp
template<std::integral Coordinate>
FurthestPairResult furthest_pair(const std::vector<std::pair<Coordinate, Coordinate>>& points);
```

平方距離が最大の 2 点を入力の添字で返す。同率の組が複数あれば、昇順にした添字の組が辞書順最小のものを選ぶ。

## 計算量

点の個数を $N$ とする。

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `furthest_pair(points)` | $O(N\log N)$ | $O(N)$ |
| `FurthestPairResult::exists()`、各フィールドの参照 | $O(1)$ | $O(1)$ |

## 注意点

- `Coordinate` は 64 bit 以下の整数型でなければならない。
- 点が 0 個または 1 個なら、`exists()` が `false` となる結果を返す。
- 2 点以上がすべて同じ座標なら、添字の組 `(0, 1)` を返す。
- 平方距離と凸包の判定は `ExactInteger` で誤差なく計算する。
