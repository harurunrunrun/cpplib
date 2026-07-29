---
title: Advanced Rectangle Packing (高度な長方形パッキング)
documentation_of: ../../../src/approximate/packing/advanced_rectangle_packing.hpp
---

## `split_fit_rectangle_packing`

```cpp
split_fit_rectangle_packing(
    rectangles, bin_width, bin_height, allow_rotation = false);
```

幅が bin 幅の半分を超える wide 群と narrow 群へ split し、各群を高さ降順に
して Bottom-Left-Fill で穴を利用する Split-Fit heuristic。時間計算量は
$O(N^3)$、空間計算量は $O(N^2)$。

## `floor_ceiling_rectangle_packing`

```cpp
floor_ceiling_rectangle_packing(
    rectangles, bin_width, bin_height, allow_rotation = false);
```

高さ降順 rectangle を、bin の floor 側と ceiling 側から交互に作る shelf へ
配置する。現在の両 shelf の空き幅を先に利用し、中央の未使用高さへ新 shelf を
作る。時間計算量は $O(NB+N\log N)$、空間計算量は $O(N+B)$。

## `steinberg_rectangle_packing`

```cpp
steinberg_rectangle_packing(
    rectangles, bin_width, bin_height, allow_rotation = false);
```

Steinberg の大 rectangle 補正項
$\max(2w-W,0)\max(2h-H,0)$ と面積の降順で処理し、短い leftover 軸を選ぶ
guillotine 分割で再帰領域を構成する Steinberg-style packing。
時間計算量は $O(N^3)$、空間計算量は $O(N^2)$。

## 注意点

寸法は正有限で、各 rectangle は許可された向きで空 bin に収まらなければならない。
返す placement は元入力順で、同一 bin 内で重ならない。これらは構築 heuristic
であり、Steinberg の十分条件を満たすすべての集合を1 binへ配置できる完全な
決定手続きや、最小 bin 数を保証するものではない。
