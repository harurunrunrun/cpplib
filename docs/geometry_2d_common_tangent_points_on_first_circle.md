---
title: 2D Common Tangent Points on First Circle (二次元第一円上共通接点) [CGL_7_G]
documentation_of: ../src/algorithm/geometry/2d/point_collection/common_tangent_points_on_first_circle.hpp
---

## API

- <code>common_tangent_points_on_first_circle(first, second)</code>: 2円の各共通接線が <code>first</code> と接する点を、<code>x</code>、<code>y</code> の辞書順で返す。返す点数は0～4個である。

## API別の時間計算量・空間計算量

- <code>common_tangent_points_on_first_circle</code>: 時間 $O(1)$、出力・追加領域 $O(1)$。

## 注意点

一致する円は共通接線が無限にあるため <code>std::domain_error</code>、負半径には <code>std::invalid_argument</code> を送出する。半径0の円も円として扱う。
