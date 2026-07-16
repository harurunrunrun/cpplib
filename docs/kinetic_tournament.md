---
title: Kinetic Tournament (動的トーナメント木)
documentation_of: ../src/structure/other/kinetic_tournament.hpp
---

添字ごとに一次関数 `slope * time + intercept` を持ち、単調に進む共通時刻での区間最小値または区間最大値を保つ。
各部分木の勝者が次に変わる時刻をcertificateとして持ち、時刻を進める際に失効した部分だけを更新する。

## 型

```cpp
KineticTournament<T, MAX_SIZE, Minimum = true>
```

`T` は符号付き整数型。`Minimum == true` なら最小値、`false` なら最大値を選ぶ。
実行時の要素数は `MAX_SIZE` 以下とする。

```cpp
struct Line { T slope, intercept; };
struct Result { T value; Line line; size_t index; };
```

同値なら、未来の時刻でも勝ち続ける傾き、それも同じなら小さい添字を選ぶ。

## API・時間計算量

- `KineticTournament(size = MAX_SIZE, initial_time = T{})`: 全要素が無効な構造を作る。時間・領域 $O(MAX\_SIZE)$。
- `size()`: 実行時の要素数を返す。時間・追加領域 $O(1)$。
- `current_time()`: 現在時刻を返す。時間・追加領域 $O(1)$。
- `active(index)`: 添字が有効かを返す。時間・追加領域 $O(1)$。
- `line(index)`: 有効なら一次関数を返し、無効なら `nullopt`。時間・追加領域 $O(1)$。
- `set(index, slope, intercept)`: 添字の一次関数を設定または置換する。時間 $O(\log(MAX\_SIZE+1))$、追加領域 $O(\log(MAX\_SIZE+1))$。
- `erase(index)`: 添字を無効にする。時間 $O(\log(MAX\_SIZE+1))$、追加領域 $O(\log(MAX\_SIZE+1))$。
- `advance_time(new_time)`: 時刻を進めてcertificateを更新する。失効して勝者が変わる木頂点数を $F$ として時間 $O((F+1)\log(MAX\_SIZE+1))$、追加領域 $O(\log(MAX\_SIZE+1))$。一次関数の列に対する時刻更新列ではcertificate変更回数に関するamortized評価を持つ。
- `query(left, right)`: 半開添字区間の勝者を返す。有効要素がなければ `nullopt`。時間・追加領域 $O(\log(MAX\_SIZE+1))$。
- `best()`: 全区間の勝者を返す。時間・追加領域 $O(1)$ 相当で、API上は `query(0,size())` を呼ぶ。

## 注意点

- `new_time` は現在時刻以上でなければならず、違反時は `invalid_argument`。
- 範囲外の添字・区間は `out_of_range`。実行時要素数が容量を超えると `length_error`。
- `slope * time + intercept` が `T` の範囲外になる結果取得は `overflow_error`。内部比較とcertificate計算には拡張整数を使う。
- 浮動小数時刻や二次以上の軌跡は扱わない。
