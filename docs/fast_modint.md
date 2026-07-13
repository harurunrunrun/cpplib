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
