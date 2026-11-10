---
title: Tetration Mod (テトレーションの剰余) [tetration_mod]
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/tetration_mod.hpp
---

## `tetration_mod`

```cpp
std::uint32_t tetration_mod(
    std::uint64_t base,
    std::uint64_t height,
    std::uint32_t modulus
)
```

高さ `height` のテトレーション

$$
\operatorname{tower}(a,0)=1,\qquad
\operatorname{tower}(a,h)=a^{\operatorname{tower}(a,h-1)}
$$

（$a$ は `base`）を `modulus` で割った余りを返す。$0^0 = 1$ とする。`modulus == 0` なら `std::invalid_argument` を送出する。関数はグローバル名前空間に定義されている。

オイラーのトーシェント関数の列に沿って指数を縮約する。各段では指数が $\varphi(m)$ 以上かを別途判定するので、底と法が互いに素でない場合も扱える。

## 時間計算量・空間計算量

$m_0 = \mathtt{modulus}$、$m_{i+1} = \varphi(m_i)$、$m_L = 1$ とする。

- 時間計算量: $O\left(\sum_{i=0}^{L-1}(\sqrt{m_i}+\log m_i)\right)$（$\sqrt{m_i}$ はトーシェントの試し割り）
- 追加領域: $O(L)$

$L = O(\log \mathtt{modulus})$ であり、`height` が非常に大きくてもトーシェント列の長さより深くは再帰しない。

## 注意点

法は `std::uint32_t` で、積は 64 bit 整数で計算するのでオーバーフローしない。
