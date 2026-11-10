---
title: Hemisphere Containment (半球包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/points_in_hemisphere.hpp
---

このヘッダは `points_in_hemisphere_default.hpp` と `points_in_hemisphere_with_seed.hpp` をまとめて include する、後方互換のための集約ヘッダである。利用する経路が 1 つなら、対応する個別ヘッダを直接 include すればよい。

## API

```cpp
bool points_in_hemisphere(const std::vector<Point3>& points);
bool points_in_hemisphere_with_seed(
    const std::vector<Point3>& points,
    std::uint64_t random_seed
);
```

原点から各点へ向かう方向をすべて含む閉半球が存在するとき `true` を返す。点までの距離は結果に影響しない。空集合には `true` を返す。

`points_in_hemisphere_with_seed` は、Seidel 法で制約を挿入する順序を決めるシードを明示的に受け取る。同じ入力とシードには同じ結果を返す。期待線形時間の保証を利用する場合は、入力の値と並びから独立に一様に選んだシードを渡す。`points_in_hemisphere` は固定シード `0xD1B54A32D192ED03` を渡す、再現可能な互換 API である。

## API 別の時間計算量・空間計算量

点数を $N$ とする。

- `points_in_hemisphere_with_seed`: シードが入力から独立な一様乱数なら期待時間 $O(N)$、最悪時間 $O(N^2)$、追加空間 $O(N)$。
- `points_in_hemisphere`: 最悪時間 $O(N^2)$、追加空間 $O(N)$。任意の入力に対する期待線形時間は保証しない。

非零な法線は、絶対値が最大の座標を $+1$ または $-1$ に正規化できるため、問題は 6 個の二次元線形実行可能性問題に帰着する。それぞれを Seidel のランダム化増分法で解き、最終的な候補はすべての入力方向に対して検算する。

## 注意点

- 各点は有限な非零ベクトルでなければならない。違反時は `std::invalid_argument` を送出する。
- 境界の大円上にある方向も許す閉半球の判定であり、開半球の判定とは異なる。
- 単位化した後の内積には、`long double` の丸め誤差に応じた許容幅を使う。線形計画の内部では最終判定より狭い許容幅を使い、候補を最終的な許容幅で検算し直す。
- シードは判定結果の意味や許容誤差を変えず、実行可能性の線形計画に制約を挿入する順序だけを変える。`random_seed ^ points.size()` を `std::mt19937_64` に与えて `std::shuffle` するため、入力から独立な一様シードは、各入力長に対しても一様なシードになる。
