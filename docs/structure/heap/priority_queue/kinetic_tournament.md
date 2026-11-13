---
title: Kinetic Tournament (動的トーナメント木)
documentation_of: ../../../../src/structure/heap/priority_queue/kinetic_tournament.hpp
---

添字ごとに一次関数 $\mathrm{slope} \cdot t + \mathrm{intercept}$ を持ち、単調に進む共通の時刻 $t$ での区間最小値または区間最大値を管理する。
各部分木の勝者が次に変わる時刻を証明書（certificate）として持ち、時刻を進めるときは失効した部分だけを更新する。

## 型

```cpp
KineticTournament<T, MAX_SIZE = SIZE_MAX, Minimum = true>
```

`T` は符号付き整数型。`Minimum == true` なら最小値、`false` なら最大値を選ぶ。
実行時の要素数は `MAX_SIZE` 以下とする。`MAX_SIZE` を省略（`SIZE_MAX`）すると、実行時の要素数に合わせて `std::vector` を確保する可変長版になる。

```cpp
using Dynamic = KineticTournament<long long>;
```

```cpp
struct Line { T slope, intercept; };
struct Result { T value; Line line; size_t index; };
```

値が等しい場合は、より先の時刻でも勝ち続ける傾きの方を、それも同じなら小さい添字の方を選ぶ。

## API

- `explicit KineticTournament(size_t size = MAX_SIZE, T initial_time = T{})`: 全要素が無効な構造を作る（可変長版では `size` のデフォルトは $0$）。
- `size()`: 実行時の要素数を返す。
- `current_time()`: 現在の時刻を返す。
- `active(index)`: 添字が有効かどうかを返す。
- `line(index)`: 有効なら一次関数を、無効なら `nullopt` を返す。
- `set(index, slope, intercept)`: 添字の一次関数を設定または置換する。
- `erase(index)`: 添字を無効にする。
- `advance_time(new_time)`: 時刻を進め、証明書を更新する。
- `query(left, right)`: 半開区間 $[\mathrm{left}, \mathrm{right})$ の勝者を返す。有効な要素がなければ `nullopt` を返す。
- `best()`: 全区間の勝者を返す（`query(0, size())` と同じ）。

コピーはできず、ムーブはできる。

## 時間計算量

$N$ を実行時の要素数とする。

- コンストラクタ: $O(N)$（固定長版は $O(\texttt{MAX_SIZE})$）
- `size`、`current_time`、`active`、`line`: $O(1)$
- `set`、`erase`: $O(\log(N+1))$
- `advance_time`: 失効して勝者が変わる木の頂点数を $F$ として $O((F+1)\log(N+1))$。一次関数の列に対する時刻の更新列では、証明書の変更回数に関する償却評価が成り立つ
- `query`: $O(\log(N+1))$
- `best`: 根の勝者を読むだけなので $O(1)$

## 空間計算量

- 本体: $O(N)$（固定長版は $O(\texttt{MAX_SIZE})$）
- `set`、`erase`、`advance_time`、`query` の追加領域: 再帰の深さ $O(\log(N+1))$
- それ以外の追加領域: $O(1)$

## 注意点

- `new_time` は現在の時刻以上でなければならない。違反すると `std::invalid_argument` を送出する。
- 範囲外の添字・区間は `std::out_of_range` を、実行時の要素数が容量（または `INT_MAX`）を超える場合は `std::length_error` を送出する。
- $\mathrm{slope} \cdot t + \mathrm{intercept}$ が `T` の範囲外になる結果の取得は `std::overflow_error` を送出する。内部の比較と証明書の計算には拡張整数を使う。
- 浮動小数点数の時刻や、2 次以上の軌跡は扱わない。
