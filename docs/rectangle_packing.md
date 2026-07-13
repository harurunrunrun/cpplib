---
title: Two-Dimensional Rectangle Packing Heuristics (二次元長方形詰め込みヒューリスティック)
documentation_of: ../src/approximate/packing/rectangle_packing.hpp
---

幅 `bin_width`、高さ `bin_height` の同一な複数 bin へ長方形を詰める
heuristic を `approximate::packing` 名前空間で提供する。入力順を保った逐次配置であり、
最小 bin 数は保証しない。

## 共通型

```cpp
template<class Size>
struct RectangleSize {
    Size width, height;
};

template<class Size>
struct RectanglePlacement {
    size_t bin;
    Size x, y, width, height;
    bool rotated;
};

template<class Size>
struct RectanglePackingResult {
    vector<RectanglePlacement<Size>> placements;
    size_t used_bin_count;
    Size bin_width, bin_height;
    size_t bin_count() const;
};
```

`placements[i]` は入力 `i` の配置である。原点は bin の左下、区間は半開区間とする。
`rotated=true` のとき入力の幅と高さを交換している。`bin_count()` は $O(1)$、
全配置の列挙は $O(N)$。

全 API で `allow_rotation=true` とすると 90 度回転も候補にする。同点では入力順、
bin 番号、候補位置、非回転を優先する。bin または長方形の寸法が非正・非有限、
あるいは許された向きで空 bin に入らない長方形がある場合は
`std::invalid_argument`。`Size` は有限な全順序付き加減算型で、`long double` へ変換できる
ものとする。座標は bin 寸法以下の値だけを加算し、面積 score は bin 面積で正規化して
計算する。

## Shelf

```cpp
enum class ShelfFit {
    next_fit, first_fit, best_width_fit, best_height_fit
};

shelf_rectangle_packing(rectangles, bin_width, bin_height,
                        fit, allow_rotation = false,
                        decreasing_height = false);
shelf_next_fit(rectangles, bin_width, bin_height, allow_rotation = false);
shelf_first_fit(rectangles, bin_width, bin_height, allow_rotation = false);
shelf_best_width_fit(rectangles, bin_width, bin_height, allow_rotation = false);
shelf_best_height_fit(rectangles, bin_width, bin_height, allow_rotation = false);
nfdh_rectangle_packing(rectangles, bin_width, bin_height,
                       allow_rotation = false);
ffdh_rectangle_packing(rectangles, bin_width, bin_height,
                       allow_rotation = false);
bfdh_rectangle_packing(rectangles, bin_width, bin_height,
                       allow_rotation = false);
```

Shelf は同じ下端と shelf 高を持つ横一列である。

- Next Fit は最後の bin の最後の shelf だけを試す。
- First Fit は入る shelf のうち `(bin, shelf)` が辞書順最小のものを選ぶ。
- Best Width Fit は配置後の shelf 残幅を最小化する。
- Best Height Fit は shelf 高と長方形高の差を最小化する。

どの既存 shelf にも入らないときは、下から新しい shelf を作る。
NFDH、FFDH、BFDH はそれぞれ高さ・幅の降順へ安定 sort してから Next、First、
Best Width Fit を行う。回転を許す場合も sort key は入力時の高さ、次に幅である。

Next Fit は $O(N)$、First/Best Fit は $O(N^2)$。decreasing 版には
$O(N\log N)$ の sort が加わる。追加領域はすべて $O(N)$。

## Bottom-Left / Bottom-Left-Fill

```cpp
bottom_left_rectangle_packing(rectangles, bin_width, bin_height,
                              allow_rotation = false,
                              fill_holes = false);
bottom_left_fill_rectangle_packing(rectangles, bin_width, bin_height,
                                   allow_rotation = false);
```

Bottom-Left は既配置長方形の右下・左上と両座標軸上の候補から、`(y, x)` が最小の
非交差位置を最初に入る bin で選ぶ。`fill_holes=true` の Bottom-Left-Fill は、全ての
右辺 x 座標と上辺 y 座標の直積も調べるため、内部の空隙を探索できる。

Bottom-Left は $O(N^3)$ 時間、Bottom-Left-Fill は $O(N^4)$ 時間、追加領域は
どちらも $O(N^2)$（Fill しない場合は $O(N)$）。

## Skyline

```cpp
enum class SkylineFit { bottom_left, best_fit };

skyline_rectangle_packing(rectangles, bin_width, bin_height,
                          fit, allow_rotation = false);
skyline_bottom_left_rectangle_packing(rectangles, bin_width, bin_height,
                                      allow_rotation = false);
skyline_best_fit_rectangle_packing(rectangles, bin_width, bin_height,
                                   allow_rotation = false);
```

各 bin の上端を高さ一定の区間列として保持する。Bottom-Left は配置後の上端、次に
x 座標を最小化する。Best-Fit は長方形の下に生じる正規化空隙面積、次に配置後の上端を
最小化する。時間計算量は $O(N^3)$、追加領域は $O(N)$。

## MaxRects

```cpp
enum class MaxRectsFit {
    best_short_side_fit, best_long_side_fit, best_area_fit
};

maxrects_rectangle_packing(rectangles, bin_width, bin_height,
                           fit, allow_rotation = false);
maxrects_best_short_side_fit(rectangles, bin_width, bin_height,
                            allow_rotation = false);
maxrects_best_long_side_fit(rectangles, bin_width, bin_height,
                           allow_rotation = false);
maxrects_best_area_fit(rectangles, bin_width, bin_height,
                      allow_rotation = false);
```

空き長方形を分割し、包含される空き長方形を除く。BSSF は短辺の余り、BLSF は長辺の
余り、BAF は面積の余りを最小化する。全 bin の空き長方形数の最大値を $F$ とすると、
包含除去を含む時間計算量は $O(NF^2)$、追加領域は $O(N+F)$。

## Guillotine Best Area Fit

```cpp
enum class GuillotineSplit {
    shorter_leftover_axis, longer_leftover_axis,
    shorter_axis, longer_axis
};

guillotine_best_area_fit_rectangle_packing(
    rectangles, bin_width, bin_height,
    allow_rotation = false,
    split = GuillotineSplit::shorter_leftover_axis
);
```

面積の余りが最小の空き長方形を選び、配置位置から端まで通る一回の guillotine cut で
二つの空き長方形へ分ける。`shorter/longer_leftover_axis` は配置後の幅・高さの余り、
`shorter/longer_axis` は分割前の空き長方形の幅・高さで切断方向を決める。
空き長方形は互いに交差せず総数は $O(N)$ なので、時間計算量は $O(N^2)$、追加領域は
$O(N)$。

全手法が返すのは常に bin 内かつ互いに内部が交差しない配置である。heuristic のため
最適 bin 数や一定の近似比は保証しない。
