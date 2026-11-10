---
title: Coordinate Product Knight Distances (座標積コストのナイト最短路) [CCHESS]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/specialized/coordinate_product_knight_distances.hpp
---

正方形の盤上でのナイトの移動について、移動前後の座標の積をコストとする最短距離を問い合わせる。

## `CoordinateProductKnightDistances`

```cpp
class CoordinateProductKnightDistances {
public:
    explicit CoordinateProductKnightDistances(int board_size);

    int board_size() const;
    std::optional<long long> distance(
        int start_x,
        int start_y,
        int target_x,
        int target_y
    ) const;
    void clear_cache();
};
```

盤の一辺の長さを $B$ とする。座標 $(x, y)$ から $(x', y')$ への 1 回のナイト移動のコストを $xx' + yy'$ とする。座標は各軸とも $[0, B)$ の範囲である。

## コンストラクタ

盤とナイトの全有向移動を構築する。構築直後は計算済みの始点がない状態である。

## `board_size`

盤の一辺の長さを返す。

## `distance`

始点 `(start_x, start_y)` から目標 `(target_x, target_y)` への最短コストを返す。到達不能なら `std::nullopt` を返す。始点ごとに Dijkstra 法の結果をキャッシュし、`clear_cache` を呼ぶまで保持する。

## `clear_cache`

保持している最短路計算結果を破棄する。盤は変更しない。

## 計算量

計算結果を保持している相異なる始点の個数を $C$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(B^2)$ | $O(B^2)$ |
| `board_size()` | $O(1)$ | $O(1)$ |
| `distance(...)`（未計算の始点） | $O(B^2 \log (B + 1))$ | $O(B^2)$ |
| `distance(...)`（計算済みの始点） | $O(1)$ | $O(1)$ |
| `clear_cache()` | $O(CB^2 + B^2)$ | $O(1)$ |

オブジェクト全体では $O((C + 1)B^2)$ 空間を使う。

## 注意点

- コンストラクタの `board_size` は $[1, 46340]$ の範囲内でなければならない。
- 各座標は盤内になければならない。
- これらの前提に違反すると `runtime_error` を送出する。
- `distance` は `const` メンバ関数だが、内部のキャッシュを更新する（`mutable`）。複数スレッドから同時に呼んではならない。
