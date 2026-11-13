---
title: Rectangle Packing Common API (長方形パッキング共通 API)
documentation_of: ../../../src/approximate/packing/rectangle_packing_common.hpp
---

2 次元ビンパッキングのヒューリスティックが共通で使う入力型・配置型・返値型である。

## API

### `RectangleSize<Size>`

入力長方形の `width`, `height` を保持する。構築・参照の時間計算量と追加空間計算量は $O(1)$。

### `RectanglePlacement<Size>`

配置先のビン番号 `bin`、左下座標 `x`, `y`、配置後の `width`, `height`、90 度回転したかどうか `rotated` を保持する。構築・参照の時間計算量と追加空間計算量は $O(1)$。

### `RectanglePackingResult<Size>`

入力の添字順に並んだ配置 `placements`、使用ビン数 `used_bin_count`、ビンの寸法 `bin_width`, `bin_height` を保持する。長方形数を $N$ として空間計算量は $O(N)$。

### `RectanglePackingResult::bin_count()`

`used_bin_count` を返す。時間計算量と追加空間計算量は $O(1)$。

## 注意点

- `rectangle_packing_internal` 名前空間の関数は各アルゴリズムの内部実装であり、互換性は保証しない。
- 公開アルゴリズムでは、`Size` に比較・四則演算・`long double` への変換を要求する。寸法は正の有限値でなければならない。
