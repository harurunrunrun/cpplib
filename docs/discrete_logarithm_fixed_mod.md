---
title: Discrete Logarithm (Fixed Mod) (法固定の離散対数) [discrete_logarithm_fixed_mod]
documentation_of: ../src/algorithm/math/number_theory/discrete_logarithm_fixed_mod.hpp
---

同じ素数法 $P$ と原始根 $g$ に対して多数の離散対数を求める前処理付きデータ構造。
$1\le x<P$ に対し、$g^b\equiv x\pmod P$ を満たす唯一の
$0\le b<P-1$ を返す。

`discrete_log.hpp` の各クエリごとの baby-step giant-step とは異なり、
Farey 近似で各クエリを定数個の表引きへ変換する。
固定された法と底に対して最大 $10^6$ 件程度の問い合わせを処理する用途を想定する。

## API

### `FixedModDiscreteLogarithm(P, g)`

前処理を行う。`P` は $2\le P\le 10^9$ の素数、`g` は `P` の原始根でなければならない。
条件を満たさない場合は `std::invalid_argument` を送出する。

### `modulus()`

前処理に使った `P` を返す。

### `primitive_root()`

前処理に使った `g` を返す。

### `log(x)` / `operator()(x)`

$g^b\equiv x\pmod P$ を満たす $0\le b<P-1$ を返す。
`x` は $1\le x<P$ でなければならず、範囲外では `std::invalid_argument` を送出する。

### `logs(values)`

`values` の各要素に `log` を適用し、同じ順序の `std::vector<std::uint32_t>` を返す。

## API別の時間計算量・空間計算量

$R$ を $\sqrt[3]P$ 以上で最小の実装上の2冪、
$K=\pi(\sqrt P)$ とする。ハッシュ表の操作を期待 $O(1)$ と数える。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | 期待 $O(R^2+\sqrt{PK})$ | $O(R^2+\sqrt{PK})$ |
| `modulus()` | $O(1)$ | $O(1)$ |
| `primitive_root()` | $O(1)$ | $O(1)$ |
| `log(x)` | $O(1)$ | $O(1)$ |
| `operator()(x)` | $O(1)$ | $O(1)$ |
| `logs(values)` | $O(\lvert values\rvert)$ | 返り値に $O(\lvert values\rvert)$ |

小さい `P` では全要素の対数表を直接構築するため、コンストラクタは時間・空間とも
$O(P)$ となる。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
