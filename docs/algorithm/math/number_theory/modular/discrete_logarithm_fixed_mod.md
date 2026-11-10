---
title: Discrete Logarithm (Fixed Mod) (法固定の離散対数) [discrete_logarithm_fixed_mod]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/discrete_logarithm_fixed_mod.hpp
---

同じ素数法 $P$ と原始根 $g$ に対して多数の離散対数を求める、前処理付きのデータ構造。$1 \le x < P$ に対し、$g^b \equiv x \pmod P$ を満たす唯一の $0 \le b < P-1$ を返す。

`discrete_log.hpp` のようにクエリごとに baby-step giant-step を行うのではなく、Farey 数列による近似で各クエリを定数個の表引きに帰着する。法と底を固定して $10^6$ 件程度までのクエリを処理する用途を想定する。

## API

### コンストラクタ `math::FixedModDiscreteLogarithm(P, g)`

前処理を行う。引数は `std::uint32_t`。`P` は $2 \le P \le 10^9$ の素数、`g` は `P` の原始根でなければならず、条件を満たさない場合は `std::invalid_argument` を送出する。

### `modulus()`

前処理に使った `P` を返す。

### `primitive_root()`

前処理に使った `g` を返す。

### `log(x)` / `operator()(x)`

$g^b \equiv x \pmod P$ を満たす $0 \le b < P-1$ を返す。`x` は $1 \le x < P$ でなければならず、範囲外では `std::invalid_argument` を送出する。

### `logs(values)`

`std::vector<std::uint32_t>` の各要素に `log` を適用し、同じ順序の `std::vector<std::uint32_t>` を返す。

## 時間計算量・空間計算量

$R$ を $\lfloor \sqrt[3]{P} \rfloor$ より大きい最小の 2 の冪、$K = \pi(\sqrt P)$（$\sqrt P$ 以下の素数の個数）とする。ハッシュ表の操作は期待 $O(1)$ と数える。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| コンストラクタ | 期待 $O(R^2 + \sqrt{PK})$ | $O(R^2 + \sqrt{PK})$ |
| `modulus()` | $O(1)$ | $O(1)$ |
| `primitive_root()` | $O(1)$ | $O(1)$ |
| `log(x)` | $O(1)$ | $O(1)$ |
| `operator()(x)` | $O(1)$ | $O(1)$ |
| `logs(values)` | $O(\lvert \mathrm{values} \rvert)$ | 戻り値に $O(\lvert \mathrm{values} \rvert)$ |

$P \le 2^{20}$ のときは全要素の対数表を直接構築するので、コンストラクタは時間・空間とも $O(P)$ になる。

## 注意点

- 内部の積は 64 bit 整数で計算するので、$P \le 10^9$ の範囲でオーバーフローしない。
- 前処理や近似が内部的に失敗した場合（理論上は起きない）は `std::runtime_error` を送出する。
