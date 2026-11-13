---
title: Two-Dimensional Rectangle Packing Heuristics (二次元長方形詰め込みヒューリスティック)
documentation_of: ../../../src/approximate/packing/rectangle_packing.hpp
---

幅 `bin_width`、高さ `bin_height` の同一のビン複数個へ長方形を詰めるヒューリスティックを、`approximate::packing` 名前空間で提供する。
入力順を保った逐次配置であり、最小ビン数は保証しない。このヘッダは shelf、bottom-left、skyline、MaxRects、guillotine の各ヘッダをまとめて読み込む。

以下、長方形数を $N$ とする。

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

`placements[i]` は入力 `i` の配置である。原点はビンの左下で、領域は半開区間として扱う。
`rotated = true` のときは入力の幅と高さを入れ替えて配置している。`bin_count()` は $O(1)$、全配置の列挙は $O(N)$。

どの API でも、`allow_rotation = true` とすると 90 度回転も候補にする。同点の場合は入力順、ビン番号、候補位置、非回転の順に優先する。
座標の計算ではビン寸法以下の値だけを加算し、面積に基づく評価値はビン面積で正規化して計算する。

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

棚（shelf）は、下端と高さが共通な横一列の領域である。

- Next Fit は最後のビンの最後の棚だけを試す。
- First Fit は入る棚のうち `(bin, shelf)` が辞書順最小のものを選ぶ。
- Best Width Fit は配置後の棚の残り幅を最小化する。
- Best Height Fit は棚の高さと長方形の高さの差を最小化する。

どの既存の棚にも入らないときは、下から順に新しい棚を作る。
NFDH、FFDH、BFDH は、高さの降順（同じなら幅の降順）に安定ソートしてから、それぞれ Next Fit、First Fit、Best Width Fit を行う。回転を許す場合も、ソートのキーは入力時の高さと幅である。

時間計算量は Next Fit が $O(N)$、First Fit と Best Fit が $O(N^2)$ で、降順版はこれに $O(N \log N)$ のソートが加わる。追加空間計算量はすべて $O(N)$。

## Bottom-Left / Bottom-Left-Fill

```cpp
bottom_left_rectangle_packing(rectangles, bin_width, bin_height,
                              allow_rotation = false,
                              fill_holes = false);
bottom_left_fill_rectangle_packing(rectangles, bin_width, bin_height,
                                   allow_rotation = false);
```

Bottom-Left は、配置済み長方形の右下・左上の点とそれらを座標軸へ射影した点を候補とし、最初に入るビンの中で `(y, x)` が最小となる重ならない位置を選ぶ。
`fill_holes = true` の Bottom-Left-Fill は、すべての右辺の $x$ 座標と上辺の $y$ 座標の直積も調べるため、内部の隙間も探索できる。

時間計算量は Bottom-Left が $O(N^3)$、Bottom-Left-Fill が $O(N^4)$。追加空間計算量は Bottom-Left が $O(N)$、Bottom-Left-Fill が $O(N^2)$。

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

各ビンの上端を、高さが一定な区間の列として保持する。Bottom-Left は配置後の上端、次に $x$ 座標を最小化する。
Best-Fit は長方形の下に生じる隙間の面積（正規化したもの）、次に配置後の上端を最小化する。
時間計算量は $O(N^3)$、追加空間計算量は $O(N)$。

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

空き長方形を分割し、ほかの空き長方形に包含されるものを取り除く。BSSF は短辺側の余り、BLSF は長辺側の余り、BAF は面積の余りを最小化する。
全ビンの空き長方形数の最大値を $F$ とすると、包含除去を含めた時間計算量は $O(NF^2)$、追加空間計算量は $O(N + F)$。

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

面積の余りが最小の空き長方形を選び、配置位置から端まで通る 1 回のギロチンカットで 2 つの空き長方形に分ける。
`shorter_leftover_axis` / `longer_leftover_axis` は配置後の幅・高さの余りで、`shorter_axis` / `longer_axis` は分割前の空き長方形の幅・高さで切断方向を決める。
空き長方形は互いに交差せず総数は $O(N)$ なので、時間計算量は $O(N^2)$、追加空間計算量は $O(N)$。

## 注意点

- ビンまたは長方形の寸法が正の有限値でない場合や、許された向きで空のビンに入らない長方形がある場合は `std::invalid_argument` を送出する。
- `Size` は有限な全順序付きの加減算可能な型で、`long double` へ変換できるものとする。
- どの手法も、ビン内に収まり互いの内部が交差しない配置を必ず返す。ただしヒューリスティックであり、最適ビン数や一定の近似比は保証しない。
