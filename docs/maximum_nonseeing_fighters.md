---
title: Maximum Non-seeing Fighters (相互不可視配置最大数) [ANGELS]
documentation_of: ../src/algorithm/matching/maximum_nonseeing_fighters.hpp
---

部屋の格子へ、上下左右に互いが見えないよう配置できる人数の最大値を返す。

# maximum_nonseeing_fighters

```cpp
int maximum_nonseeing_fighters(const vector<string>& rooms);
```

`H` は配置可能な透明の空室、`A` は視線を遮る既使用の不透明室、`D` は配置不能だが視線を遮らない透明室を表す。`H` に配置する人同士が同じ水平・垂直の透明区間に存在しないよう選べる最大室数を返す。

## API別の時間計算量・空間計算量

格子の行数を $R$、列数を $C$、`H` の個数を $E$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_nonseeing_fighters` | $O(RC+E\sqrt{RC})$ | $O(RC)$ |

## 注意点

- 全行の長さは等しく、各文字は `H`, `A`, `D` のいずれかでなければならない。
- 前提違反時は `runtime_error` を送出する。
- 空の格子には0を返す。
