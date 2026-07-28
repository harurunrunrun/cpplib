---
title: Furthest Pair Result (最遠点対の結果)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/furthest_pair_result.hpp
---

整数点の最遠点対を入力添字と正確な平方距離で保持する結果型。

## API

```cpp
struct FurthestPairResult {
    std::size_t first;
    std::size_t second;
    ExactInteger squared_distance;
    bool exists() const;
};
```

- `first`, `second`: 組が存在するとき `first < second` となる入力添字。
- `squared_distance`: 2点間の平方距離。
- `exists()`: 有効な組を保持していればtrue。

既定構築では両添字が `std::numeric_limits<std::size_t>::max()`、平方距離が0で、
`exists()` はfalseとなる。

## 時間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 既定構築、各fieldの参照 | $O(1)$ | $O(1)$ |
| `exists()` | $O(1)$ | $O(1)$ |

## 注意点

`squared_distance` は任意精度整数 `ExactInteger` で常に非負である。
`exists()` がfalseのとき、平方距離と添字を有効な点対として使用してはならない。
