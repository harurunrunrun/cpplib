---
title: Fast Modint (高速剰余整数)
documentation_of: ../src/structure/modint/fast_modint.hpp
---

固定 mod の整数型。

乗算に Barrett reduction を使う。

# テンプレート引数

```cpp
FastModint<mod>
```

`mod >= 1`が必要。値は常に`[0,mod)`に正規化される。

# 関数

```cpp
FastModint()
FastModint(long long value)
static constexpr int get_mod()
static FastModint raw(int value)
int val() const
FastModint pow(long long exponent) const
FastModint inv() const
```

整数constructorは`value mod mod`を作り、`val()`は正規化済み代表値を返す。
`raw(value)`は正規化を省略するため、`0 <= value < mod`が検査されない前提。

`pow`は非負の`exponent`乗を返し、負なら例外。
`inv`は乗法逆元を返す。素数法で値が0の場合、または合成数法で値と法が
互いに素でない場合は例外。除算も右辺の`inv()`を使う。

stream入力は符号付き64bit整数を読み、入力に失敗すると例外。
stream出力は`val()`を10進出力する。

# 演算

```cpp
単項 +, 単項 -, ++, --, +, -, *, /,
+=, -=, *=, /=, ==, !=, operator>>, operator<<
```

# エイリアス

```cpp
FastModint998244353
FastModint1000000007
```

## 時間計算量

入出力については、整数の桁数を $D$ とする。

| 操作 | 時間計算量 |
| --- | --- |
| コピー・move構築、コピー・move代入 | $O(1)$ |
| デフォルト構築、`FastModint(v)` | $O(1)$ |
| `get_mod()`, `raw(v)`, `val()` | $O(1)$ |
| `+=`, `-=`, `*=`, `+`, `-`, `*` | $O(1)$ |
| 単項 `+`, 単項 `-`, `++`, `--` | $O(1)$ |
| `==`, `!=` | $O(1)$ |
| `pow(n)` | $O(\log(n+1))$ |
| `inv()`, `/=`, `/` | $O(\log mod)$ |
| `operator>>`, `operator<<` | $O(D)$（ストリーム自身の処理を含む） |

## 注意点

`mod >= 1`。`raw(v)` は正規化と検査を省略するため `0 <= v < mod` を呼出側が保証する。
`pow` の指数は非負。`inv` と除算は逆元の存在を要求し、0または法と非互いに素な値では
`runtime_error`。入力失敗も `runtime_error`。
オブジェクトの保存領域と、入出力以外の各APIの追加空間計算量は $O(1)$。
`pow`, `inv`, 除算は反復実装でstackを増やさない。

## 演算APIの詳細

- `operator+=(rhs)`：`rhs` を加え、`[0,mod)` に戻す。$O(1)$。
- `operator-=(rhs)`：`rhs` を引き、`[0,mod)` に戻す。$O(1)$。
- `operator*=(rhs)`：Barrett reductionで積を正規化する。$O(1)$。
- `operator/=(rhs)`：`rhs.inv()` を掛ける。$O(\log mod)$。
- 単項 `operator+()`：同じ値を返す。$O(1)$。
- 単項 `operator-()`：加法逆元を返す。$O(1)$。
- 前置 `operator++()` / `operator--()`：1を加減し、0と `mod-1` の間で巡回する。$O(1)$。
- 後置 `operator++(int)` / `operator--(int)`：変更前の値を返してから1を加減する。$O(1)$。
- 二項 `operator+`, `operator-`, `operator*`：左辺のcopyへ対応するcompound演算を行う。$O(1)$。
- 二項 `operator/`：左辺のcopyを右辺で割る。$O(\log mod)$。
- `operator==` / `operator!=`：正規化済み代表値を比較する。$O(1)$。
- `operator>>(istream,x)`：符号付き64bit整数を読み、`x`へ正規化して代入する。桁数を $D$ として $O(D)$。
- `operator<<(ostream,x)`：`x.val()` を10進出力する。桁数を $D$ として $O(D)$。

全演算の返却値以外の追加空間計算量は $O(1)$。compound演算と前置increment/decrementは
`*this`、後置increment/decrementと二項演算は値を返す。入力演算は成功時にstream、
失敗時に `runtime_error` を送出する。
