---
title: Closest Pair Result (最近点対の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/closest_pair_result.hpp
---

整数点の最近点対を、入力添字と厳密な距離の二乗で保持する結果型。

## API

```cpp
struct ClosestPairResult {
    std::size_t first;
    std::size_t second;
    ExactInteger squared_distance;
    bool exists() const;
};
```

- `first`, `second`: 組が存在するとき `first < second` となる入力添字。
- `squared_distance`: 2 点間の距離の二乗。
- `exists()`: 有効な組を保持していれば（`first` が無効値でなければ）`true` を返す。

デフォルト構築では両添字が `std::numeric_limits<std::size_t>::max()`、距離の二乗が $0$ で、
`exists()` は `false` となる。

## 時間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| デフォルト構築、各フィールドの参照 | $O(1)$ | $O(1)$ |
| `exists()` | $O(1)$ | $O(1)$ |

## 注意点

`squared_distance` は任意精度整数 `ExactInteger` で、常に非負である。
`exists()` が `false` のとき、距離の二乗と添字を有効な点対として使ってはならない。
