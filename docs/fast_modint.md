---
title: Fast Modint
documentation_of: ../src/structure/modint/fast_modint.hpp
---

固定 mod の整数型。

乗算に Barrett reduction を使う。

# テンプレート引数

```cpp
FastModint<mod>
```

# 関数

```cpp
FastModint x
FastModint::raw(v)
x.val()
x.pow(n)
x.inv()
```

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
| コピー・move構築、コピー・move代入 | $O(1)$ |
| --- | --- |
| デフォルト構築、`FastModint(v)` | $O(1)$ |
| `get_mod()`, `raw(v)`, `val()` | $O(1)$ |
| `+=`, `-=`, `*=`, `+`, `-`, `*` | $O(1)$ |
| 単項 `+`, 単項 `-`, `++`, `--` | $O(1)$ |
| `==`, `!=` | $O(1)$ |
| `pow(n)` | $O(\log(n+1))$ |
| `inv()`, `/=`, `/` | $O(\log mod)$ |
| `operator>>`, `operator<<` | $O(D)$（ストリーム自身の処理を含む） |
