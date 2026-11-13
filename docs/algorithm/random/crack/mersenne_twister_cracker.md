---
title: Mersenne Twister State Cracker (メルセンヌツイスター状態復元)
documentation_of: ../../../../src/algorithm/random/crack/mersenne_twister_cracker.hpp
---

連続した完全出力のテンパリングを逆変換し、Mersenne Twister の内部状態列を復元する。

## MersenneTwisterCracker

~~~cpp
MersenneTwisterCracker<
    UInt, WordSize, StateSize, ShiftSize, MaskBits, XorMask,
    TemperingU, TemperingD, TemperingS, TemperingB,
    TemperingT, TemperingC, TemperingL
> cracker(outputs);
~~~

`UInt` は符号なし整数、`WordSize` は 1 以上 `UInt` のビット数以下、`StateSize` は 1 以上、`ShiftSize` は 1 以上 `StateSize` 以下、`MaskBits` は `WordSize` 以下、各テンパリングのシフト量は 1 以上 `WordSize` 以下でなければならない（`static_assert`）。

`outputs` は `std::array<result_type, Count>` または `std::span<const result_type>` で、要素数は `StateSize` 以上でなければならない（`array` 版は `static_assert`、`span` 版は不足すると `std::invalid_argument`）。先頭 `StateSize` 語から状態を作り、余分な語は再生して整合性を検査する。`WordSize` を超える上位ビットが立っている場合、または整合性検査に失敗した場合は `std::invalid_argument` を送出する。

- `result_type`: 生成値に使う `UInt`。
- `state_type`: 長さ `StateSize` の状態配列。
- コンストラクタ: 連続出力から状態を復元し、余分な出力があれば照合する。
- `next()` / `operator()()`: 次の完全出力を返す。
- `discard(count)`: `count` 出力分進める。
- `state()`: 次の予測に必要なテンパリング前の状態を古い順で返す。
- `min()` / `max()`: 下限 0 と、`WordSize` ビットがすべて 1 の上限を返す。

`Mt19937Cracker` は 32 bit・624 語、`Mt19937_64Cracker` は 64 bit・312 語の別名である。

## 時間計算量

状態語数を $N$、追加で検査する語数を $K$ とする。

- コンストラクタ: $O(N+K)$ 時間・$O(N)$ 保存領域
- `next` / `operator()`: $O(1)$
- `discard(count)`: $O(\mathtt{count})$
- `state()`: $O(N)$ 時間・$O(N)$ 戻り値領域
- `min` / `max`: $O(1)$
- 型別名の参照: $O(1)$

各生成操作の追加空間計算量は $O(1)$。

## 注意点

- 観測開始位置は 624 語ブロックの境界でなくてもよい。
- 分布変換後の値、一部のビットだけの値、呼出しの欠落を含む列には適用できない。
- 復元するのは内部状態であり、最初に渡された整数シードや `seed_seq` ではない。
- 暗号用途の乱数には使用しない。
- 観測列の長さは実行時に決めてよい。`span` 版は観測列をコピーしない。

~~~cpp
std::mt19937 generator(123);
std::vector<std::uint32_t> outputs(700);
for(auto& value: outputs) value = generator();
Mt19937Cracker cracker{std::span<const std::uint32_t>(outputs)};
auto next_value = cracker.next();
~~~
