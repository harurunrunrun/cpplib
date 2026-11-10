---
title: Normalized Line Aggregator (正規化直線集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/normalized_line.hpp
---

正規化した直線の結果型と、直線を入力とする API をまとめた後方互換の集約ヘッダである。既存のコードはこのヘッダをそのまま利用できる。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `normalized_line_result.hpp` | `NormalizedLine` |
| `normalized_line_line.hpp` | `normalized_line(line)` |

## 集約される API

```cpp
struct NormalizedLine {
    long double a, b, c;
};

NormalizedLine normalized_line(const Line& line);
```

直線を $ax + by + c = 0$ の形で、$a^2 + b^2 = 1$ かつ（$a \gt 0$、または $a = 0$ かつ $b \ge 0$）となるよう正規化した係数を返す。同じ直線なら、通る 2 点の選び方や向きによらずほぼ同じ係数になる。

## 計算量

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `normalized_line`、係数の参照 | $O(1)$ | $O(1)$ |

## 注意点

退化した直線に対しては `std::invalid_argument` を送出する。
