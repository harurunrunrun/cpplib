---
title: Advanced Rectangle Packing (高度な長方形パッキング)
documentation_of: ../../../src/approximate/packing/advanced_rectangle_packing.hpp
---

以下、長方形の数を $N$、生成したビンの数を $B$、ビンの幅・高さを $W, H$ とする。

## `split_fit_rectangle_packing`

```cpp
split_fit_rectangle_packing(
    rectangles, bin_width, bin_height, allow_rotation = false);
```

幅がビンの幅の半分を超える「幅広」群とそれ以外の「幅狭」群に分け、各群を高さの降順に並べて、Bottom-Left-Fill で隙間を利用しながら配置する Split-Fit ヒューリスティックである。時間計算量は $O(N^3)$、空間計算量は $O(N^2)$ である。

## `floor_ceiling_rectangle_packing`

```cpp
floor_ceiling_rectangle_packing(
    rectangles, bin_width, bin_height, allow_rotation = false);
```

高さの降順に並べた長方形を、ビンの床側と天井側から交互に作る棚（shelf）へ配置する。現在の両方の棚の空き幅を先に利用し、なければ中央の未使用の高さに新しい棚を作る。時間計算量は $O(NB+N\log N)$、空間計算量は $O(N+B)$ である。

## `steinberg_rectangle_packing`

```cpp
steinberg_rectangle_packing(
    rectangles, bin_width, bin_height, allow_rotation = false);
```

Steinberg の大きな長方形に対する補正項 $\max(2w-W,0)\max(2h-H,0)$ と面積の降順で処理し、余りの短い方の軸で切るギロチン分割で再帰的な領域を構成する、Steinberg 型のパッキングである。時間計算量は $O(N^3)$、空間計算量は $O(N^2)$ である。

## 注意点

- 寸法は正の有限値で、各長方形は許可された向きで空のビンに収まらなければならない
- 返す配置は元の入力順で、同じビン内では互いに重ならない
- これらは構築型のヒューリスティックであり、Steinberg の十分条件を満たすすべての集合を 1 つのビンに配置できる完全な決定手続きではなく、最小のビン数も保証しない
