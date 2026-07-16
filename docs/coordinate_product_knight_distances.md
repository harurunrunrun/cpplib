---
title: Coordinate Product Knight Distances (座標積コストのナイト最短路) [CCHESS]
documentation_of: ../src/structure/graph/coordinate_product_knight_distances.hpp
---

正方形盤上のナイト移動について、移動前後の座標積をコストとする最短距離を問い合わせる。

## CoordinateProductKnightDistances

```cpp
class CoordinateProductKnightDistances {
public:
    explicit CoordinateProductKnightDistances(int board_size);

    int board_size() const;
    optional<long long> distance(
        int start_x,
        int start_y,
        int target_x,
        int target_y
    ) const;
    void clear_cache();
};
```

座標 $(x,y)$ から $(x',y')$ への1回のナイト移動のコストを $xx'+yy'$ とする。座標は各軸とも $[0,\mathit{board\_size})$ である。

## Constructor

盤とナイトの全有向移動を構築する。計算済みの始点はない状態になる。

## board_size

盤の一辺の長さを返す。

## distance

指定した2マス間の最短コストを返す。到達不能なら `nullopt` を返す。同じ始点の結果は `clear_cache` まで保持する。

## clear_cache

保持している最短路計算結果を破棄する。盤は変更しない。

## API別の時間計算量・空間計算量

盤の一辺を $B$、計算結果を保持している異なる始点数を $C$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(B^2)$ | $O(B^2)$ |
| `board_size()` | $O(1)$ | $O(1)$ |
| `distance(...)`（未計算の始点） | $O(B^2\log(B+1))$ | $O(B^2)$ |
| `distance(...)`（計算済みの始点） | $O(1)$ | $O(1)$ |
| `clear_cache()` | $O(CB^2+B^2)$ | $O(1)$ |

構造体全体では $O((C+1)B^2)$ 空間を使う。

## 注意点

- `board_size` は $[1,46340]$ でなければならない。
- 各座標は盤内でなければならない。
- 前提違反時は `runtime_error` を送出する。
