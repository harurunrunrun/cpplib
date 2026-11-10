---
title: Linear Congruential Generator (線形合同法乱数生成器)
documentation_of: ../../../../src/algorithm/random/engine/linear_congruential_generator.hpp
---

$x_{i+1}=Ax_i+C\pmod M$ で状態を更新する線形合同法乱数生成器。

~~~cpp
LinearCongruentialGenerator<UInt, A, C, M> engine(seed)
~~~

`UInt` は 64bit 以下の符号なし整数。$M$ は 0 または 2 以上で、$M>0$ では $A,C<M$ が必要。`M == 0` は $2^W$（$W$ は `UInt` のビット数）を法とし、符号なし整数の桁あふれを利用する。

## 公開 API

- `result_type`：生成値と内部状態に使う型 `UInt`。
- `multiplier` / `increment` / `modulus`：それぞれ $A,C,M$ を表す静的定数。
- `default_seed`：既定シードの 1。
- 既定コンストラクタ：シード 1 で構築する。
- `LinearCongruentialGenerator(seed)` / `seed(value)`：$M>0$ では `value % M` を状態にする。$C=0$ かつ正規化後が 0 なら状態 1 を使う。引数を省略した `seed()` は既定シードへ戻す。
- `transition(state)`：1 回遷移した状態を返す。生成器自身は変更しない。
- `operator()()`：1 回遷移し、更新後の状態を返す。
- `discard(count)`：`count` 回分をアフィン変換の二乗法で飛ばす。
- `state()`：現在状態を返す。
- `min()` / `max()`：生成範囲を返す。$C\ne0$ の `min()` は 0。$C=0$ では、$M=0$ かつ $A$ が奇数の場合、または $M>0$ かつ $\gcd(A,M)=1$ の場合に 1、それ以外は 0。`max()` は $M=0$ なら `std::numeric_limits<UInt>::max()`、$M>0$ なら $M-1$。

`MinStdRand0` は $(A,C,M)=(16807,0,2147483647)$、`MinStdRand` は $(48271,0,2147483647)$ の別名。

## 時間計算量

- 各コンストラクタ、`seed`、`transition`、`operator()`、`state`、`max`、各静的定数の参照：$O(1)$
- `min()`：最悪 $O(\log M)$。$C\ne0$ または $M=0$ では $O(1)$
- `discard(count)`：$O(\log count)$

追加空間計算量はすべて $O(1)$。

## 注意点

64bit 積は `unsigned __int128` で計算してから剰余を取る。内部状態をそのまま出力し、上位ビットの抽出や値域変換は行わない。暗号用途には使用しない。
