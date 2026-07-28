---
title: Chirp Z-Transform (チャープZ変換)
documentation_of: ../../../../src/algorithm/fft/transform/chirp_z_transform.hpp
---

多項式 $f$ を等比数列上の点 $z,zr,zr^2,\ldots$ で一括評価する。

## API

- `chirp_z_transform<MOD>(coefficients, initial_point, ratio, count)`:
  $k=0,\ldots,count-1$ に対する $f(initial\_point\,ratio^k)$ を返す。

## API別の時間計算量・空間計算量

$n=|coefficients|$、$m=count$、多項式乗算時間を $M$ とすると、
$O(M(n+m)+n+m)$ 時間、$O(n+m)$ 追加領域。

## 注意点

`ratio != 0` では逆元を用いるため、係数環上で `ratio` が可逆でなければ
ならない。`ratio == 0` は別処理し、最初だけ $f(initial\_point)$、以降は
$f(0)$ を返す。`count == 0` では空vector。
