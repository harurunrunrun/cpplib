---
title: Normalized Line Aggregator (正規化直線集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/normalized_line.hpp
---

正規化直線の結果型と直線入力APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `normalized_line_result.hpp` | `NormalizedLine` |
| `normalized_line_line.hpp` | `normalized_line(line)` |

## 集約されるAPI

```cpp
NormalizedLine normalized_line(const Line& line);
```

## API別の時間計算量・空間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `normalized_line`、係数の参照 | $O(1)$ | $O(1)$ |

## 注意点

退化直線では `std::invalid_argument` を送出する。
既存コードはこのヘッダを変更せず利用できる。
