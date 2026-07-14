---
title: Tetration Mod (テトレーションの剰余) [tetration_mod]
documentation_of: ../src/algorithm/math/tetration_mod.hpp
---

## `tetration_mod`

```cpp
std::uint32_t tetration_mod(
    std::uint64_t base,
    std::uint64_t height,
    std::uint32_t modulus
)
```

高さ `height` のtetration

$$
\operatorname{tower}(a,0)=1,\qquad
\operatorname{tower}(a,h)=a^{\operatorname{tower}(a,h-1)}
$$

を `modulus` で割った余りとして返す。$0^0=1$ とする。`modulus == 0` なら
`invalid_argument` を投げる。

Eulerのtotient列に沿って指数を縮約する。各段では、指数が周期へ到達したかを
別途判定するため、底とmodulusが互いに素でない場合も扱える。

## API別の時間計算量・空間計算量

$m_0=\mathtt{modulus}$、$m_{i+1}=\varphi(m_i)$、$m_L=1$ とする。

- 時間計算量: $O\left(\sum_{i=0}^{L-1}(\sqrt{m_i}+\log m_i)\right)$
- 追加領域: $O(L)$

$L=O(\log\mathtt{modulus})$。`height` が非常に大きくてもtotient列より深く再帰しない。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
