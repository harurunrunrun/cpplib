---
title: One-Dimensional Bin Packing Heuristics (一次元ビンパッキングヒューリスティック)
documentation_of: ../../../src/approximate/packing/bin_packing.hpp
---

容量の等しいビンにアイテムを詰める、オンライン型・降順型のヒューリスティックを `approximate::packing` 名前空間で提供する。以下、アイテム数を $N$、ビンの数を $B$ とする。

## `BinPackingResult`

```cpp
template<class Size>
struct BinPackingResult {
    vector<size_t> bin_of_item;
    vector<Size> remaining_capacity;
    size_t bin_count() const;
};
```

各アイテムのビン番号と、各ビンの残容量を持つ。`bin_count()` とメンバの参照の時間計算量・追加空間計算量は $O(1)$ である。全結果を列挙する時間計算量は $O(N+B)$、返り値を除く追加空間計算量は $O(1)$ である。

## 詰め込み API

```cpp
next_fit_bin_packing(items, capacity, decreasing = false)
first_fit_bin_packing(items, capacity, decreasing = false)
best_fit_bin_packing(items, capacity, decreasing = false)
worst_fit_bin_packing(items, capacity, decreasing = false)
```

- Next Fit: 現在のビンに入らなければ新しいビンを開く
- First Fit: 入るビンのうち番号が最小のものを選ぶ
- Best Fit: 詰めた後の残容量が最小になるビンを選ぶ
- Worst Fit: 詰める前の残容量が最大のビンを選ぶ

`decreasing = true` では、アイテムを大きさの降順（同じ大きさなら元の添字順）に並べてから実行する。返す `bin_of_item` は常に元のアイテムの添字に対応する。First Fit は最大値のセグメント木、Best Fit / Worst Fit は平衡二分探索木でビンを選ぶ。

## 計算量

| API | 時間計算量（`decreasing = false`） | 時間計算量（`decreasing = true`） | 追加空間計算量 |
| --- | --- | --- | --- |
| Next Fit | $O(N)$ | $O(N\log N)$ | $O(N)$ |
| First Fit | $O(N\log N)$ | $O(N\log N)$ | $O(N)$ |
| Best Fit | $O(N\log N)$ | $O(N\log N)$ | $O(N)$ |
| Worst Fit | $O(N\log N)$ | $O(N\log N)$ | $O(N)$ |

## 注意点

- 容量が正でない、アイテムが負、アイテムが容量を超える、または浮動小数点の値が有限でない場合は `std::invalid_argument` を送出する
- `Size` は全順序を持ち加減算ができる有限値の型で、減算結果を表現できなければならない
