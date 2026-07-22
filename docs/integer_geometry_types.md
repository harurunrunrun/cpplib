---
title: Integer Geometry Types (整数幾何基本型)
documentation_of: ../src/algorithm/integer_geometry/types.hpp
---

## API

- `Coordinate`, `Wide`, `UnsignedWide`: それぞれ `int64_t`、`__int128_t`、`__uint128_t` の公開別名。
- `integer_geometry::Point`: `int64_t` の `x, y` を持つ点。辞書順比較と等値比較ができる。
- `integer_geometry::Vector`: `__int128_t` の `x, y` を持つベクトル。加減算、符号反転、スカラー倍ができる。
- `integer_geometry::Segment`: 端点 `a, b` を持つ閉線分。
- `integer_geometry::Line`: 二点 `a, b` で無限直線を表す型。同一点なら退化直線として扱う。
- `vector_from(from, to)`: `from` から `to` へのベクトルを返す。
- `Rational(numerator, denominator)`: 符号と最大公約数を符号なし128 bitで処理し、分母を正にして既約化する。`INT128_MIN` を分子・分母に渡しても符号付き絶対値の未定義動作は起こさない。分母が $0$ なら `std::invalid_argument`、既約化後の分子または正の分母が `__int128_t` で表せなければ `std::overflow_error` を送出する。
- `RationalPoint`: 二つの `Rational` 座標を持つ点。
- `SegmentIntersectionKind`: `none`, `touch`, `proper`, `overlap` の線分交差種別。
- `PointLocation`: `outside`, `boundary`, `inside` の点包含種別。

## API別の時間計算量・空間計算量

- `Rational` の構築は値のbit長を $L$ として時間 $O(L)$、追加領域 $O(1)$。
- それ以外の型操作と `vector_from` は時間・追加領域 $O(1)$。

## 注意点

浮動小数点数は使わない。座標差は `__int128_t` へ変換してから計算する。ベクトル演算または有理数の正規化結果が `__int128_t` で表せない場合は `std::overflow_error` を送出し、wraparoundや符号付きoverflowは起こさない。
