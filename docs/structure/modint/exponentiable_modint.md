---
title: Exponent-Tower Modint (累乗塔対応剰余整数)
documentation_of: ../../../src/structure/modint/exponentiable_modint.hpp
---

指数にも使える剰余整数の型。

$\bmod P$, $\bmod \varphi(P)$, $\bmod \varphi(\varphi(P))$, $\dots$ の値を持つ。$P$ 以上の値は $\bmod P$ に落とした後に $P$ を足して持つため、指数が小さいのか周期に入っているのかを区別できる。

## 使い方

```cpp
using mint = ExponentiableModint<998244353>;

mint n = 4;
mint k = 3;
mint m = 2;

auto ans = m.pow(k.pow(n)).val(); // 2^(3^4) mod 998244353
```

## 補助関数

```cpp
constexpr uint32_t exponentiable_modint_totient(uint32_t x)
```

$x \ge 1$ に対する Euler のトーシェント関数 $\varphi(x)$ を試し割りで求める。
実行時に呼ぶ場合は $O(\sqrt{x})$、追加領域は $O(1)$ である。`next_type` を作るときはコンパイル時に評価される。

## メンバ関数

- `ExponentiableModint()`
    - $0$ として構築する。
- `ExponentiableModint(uint64_t x)`
    - $x$ から構築する。各段に同じ非負整数を伝播し、周期に達したかの情報も保持する。
- `ExponentiableModint(value_type value, next_type next)`
    - 現在の段と次の段の内部表現を直接指定する低レベルの API。整合性は呼び出し側が保証する。`MOD > 1` の一般のテンプレートにだけ存在する。
- `static constexpr uint32_t get_mod()`
    - 法を返す。
- `using value_type = uint32_t`
- `using next_type = ExponentiableModint<φ(MOD)>`
    - `MOD > 1` の一般のテンプレートにだけ存在する。
- `uint32_t val() const`
    - $\bmod P$ での値を返す。
- `ExponentiableModint pow(const ExponentiableModint& rhs) const`
    - $\mathrm{this}^{\mathrm{rhs}}$ を返す。Euler のトーシェント関数の列を使い、指数が周期に達したかを区別して累乗を計算する。

## 演算

- `+`、`+=`
- `*`、`*=`
- `==`、`!=`（`val()` どうしを比較する）

値が $0$ か、正か、十分大きいかの情報を保つため、減算は無い。

## 時間計算量

$P_0=P$、$P_{i+1}=\varphi(P_i)$ とし、$P_L=1$ になるまでの段数を $L$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| デフォルト構築、`ExponentiableModint(x)`、2 引数の構築 | $O(L+1)$ |
| コピー・ムーブ構築、コピー・ムーブ代入 | $O(L+1)$ |
| `get_mod()` | $O(1)$ |
| `val()` | $O(1)$ |
| `+=`、`+` | $O(L+1)$ |
| `*=`、`*` | $O(L+1)$ |
| `==`、`!=` | $O(1)$ |
| `pow(rhs)` | $O(1+\sum_{i=0}^{L-1}\log(P_i+1))$ |

$L=O(\log P)$ なので、`pow` は $O(L\log P)=O(\log^2 P)$ とも書ける。

`MOD = 1` の特殊化では、すべてのコンストラクタ・演算子・メンバ関数が $O(1)$ である。

## 空間計算量

`get_mod`、`val`、比較の追加領域は $O(1)$。一般のテンプレートのコンストラクタ、コピー・ムーブ、加算、乗算、`pow` は、トーシェント列をたどる再帰のスタックと戻り値のオブジェクトを含めて $O(L)$。`MOD = 1` の特殊化ではすべて $O(1)$ である。

## 注意点

- テンプレート引数は $1 \le \mathrm{MOD} \lt 2^{31}$ でなければならない（`static_assert` で検査する）。
- `val()` は現在の段の値を `MOD` で割った代表値を返す。
- 内部の `uint64_t` の乗算と正規化で表せるように設計されており、例外を送出する API は無い。
