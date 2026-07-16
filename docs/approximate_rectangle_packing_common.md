---
title: Rectangle Packing Common API (長方形packing共通API)
documentation_of: ../src/approximate/packing/rectangle_packing_common.hpp
---

## Rectangle Packing Common API (長方形packing共通API)

2次元bin packing heuristicが共有する入力型・配置型・返値型。

## API

### `RectangleSize<Size>`

入力長方形の `width`, `height` を保持する。構築・参照の時間計算量・追加空間計算量は $O(1)$。

### `RectanglePlacement<Size>`

配置先 `bin`、左下座標 `x,y`、配置後の `width,height`、90度回転の有無 `rotated` を保持する。構築・参照の時間計算量・追加空間計算量は $O(1)$。

### `RectanglePackingResult<Size>`

入力添字ごとの `placements`、使用bin数 `used_bin_count`、bin寸法を保持する。空間計算量は長方形数を $N$ として $O(N)$。

### `RectanglePackingResult::bin_count()`

`used_bin_count` を返す。時間計算量・追加空間計算量は $O(1)$。

## 注意点

`rectangle_packing_internal` 以下の関数は各algorithmの内部実装であり、互換性を保証しない。公開algorithmでは `Size` に比較・四則演算・`long double` への変換を要求する。寸法は正の有限値でなければならない。
