---
title: 2D Line Direction Relation (二次元直線方向関係) [CGL_2_A]
documentation_of: ../src/algorithm/geometry/2d/predicate/line_direction_relation.hpp
---

## API

- <code>line_direction_relation(first, second)</code>: 2直線の方向関係を返す。返り値は <code>LineDirectionRelation::NEITHER</code>、<code>ORTHOGONAL</code>、<code>PARALLEL</code> のいずれかで、それぞれ整数値0、1、2を持つ。

## API別の時間計算量・空間計算量

- <code>line_direction_relation</code>: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

許容誤差付きの外積・内積判定を行い、平行を先に判定する。どちらかの直線が退化している場合は <code>std::invalid_argument</code> を送出する。
