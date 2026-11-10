---
title: Closest Pair of Integer Points Aggregator (整数点の最近点対集約ヘッダ) [closest_pair]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/closest_pair_indices.hpp
---

整数点の最近点対の結果型と、点列を入力とする API をまとめて読み込む後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `closest_pair_result.hpp` | `ClosestPairResult` |
| `closest_pair_indices_points.hpp` | `closest_pair_indices(points)` |

必要な leaf header だけを直接 include することもできる。従来どおり `closest_pair_indices.hpp` を include すれば全 API を使える。

## 集約される API

```cpp
struct ClosestPairResult {
    std::size_t first, second;     // 見つからなければ std::numeric_limits<std::size_t>::max()
    ExactInteger squared_distance;
    bool exists() const;
};

template<std::integral Coordinate>
ClosestPairResult closest_pair_indices(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
);
```

平方距離が最小の 2 点を入力の添字で返す（`first < second`）。同率の組が複数あれば、添字の組 `(first, second)` が辞書順最小のものを選ぶ。

## 計算量

点の個数を $N$ とする。

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `closest_pair_indices(points)` | $O(N\log N)$ | $O(N)$ |
| `ClosestPairResult::exists()`、各フィールドの参照 | $O(1)$ | $O(1)$ |

## 注意点

- `Coordinate` は 64 bit 以下の整数型でなければならない（`static_assert` で検査する）。
- 点が 0 個または 1 個なら、`exists()` が `false` となる結果を返す。
- 平方距離は `ExactInteger` で誤差なく保持する。
