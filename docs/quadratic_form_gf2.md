---
title: Quadratic Form over GF(2) (二元体上の二次形式)
documentation_of: ../src/algorithm/math/quadratic_form_gf2.hpp
---

$\mathbb F_2$ 上の二次多項式

$$
q(x)=c+\sum_i l_i x_i+\sum_{i<j} a_{ij}x_ix_j
$$

を保持する。極形式をシンプレクティック標準形へ変換し、階数、Arf不変量、
$q(x)=0,1$ の解数を求められる。標準形は

$$
c+\sum_{i=0}^{r/2-1}(u_iv_i+\alpha_i u_i+\beta_i v_i)
  +\sum_j\gamma_jz_j
$$

である。

## `QuadraticFormGF2`

```cpp
explicit QuadraticFormGF2(size_t variable_count)
```

変数が `variable_count` 個の零多項式を構築する。

### 係数の参照と更新

```cpp
size_t variable_count() const
bool constant() const
void set_constant(bool value)
void toggle_constant()
bool linear_coefficient(size_t index) const
void set_linear(size_t index, bool value)
void toggle_linear(size_t index)
bool quadratic_coefficient(size_t first, size_t second) const
void set_quadratic(size_t first, size_t second, bool value)
void toggle_quadratic(size_t first, size_t second)
```

定数項、一次係数、二次係数を参照または更新する。係数は `bool` で表す。
$x_i^2=x_i$ なので `first == second` の二次係数は一次係数として扱う。
`toggle_*` は係数へ 1 を加える。

### `evaluate`

```cpp
bool evaluate(const vector<bool>& assignment) const
```

元の変数順の代入における $q$ の値を返す。

### `canonicalize`

```cpp
QuadraticFormGF2Canonical canonicalize() const
```

可逆な線形変数変換により標準形を作る。返り値の
`symplectic_linear[i]` は $(\alpha_i,\beta_i)$、
`radical_linear[j]` は $\gamma_j$ である。
`canonical_basis[k]` は標準形の第 $k$ 変数に対応する元変数ベクトルを、
64 bitずつpackして保持する。基底の順は
$u_0,v_0,u_1,v_1,\ldots,z_0,z_1,\ldots$ である。

## `QuadraticFormGF2Canonical`

### `bilinear_rank`

```cpp
size_t bilinear_rank() const
```

極形式 $B(x,y)=q(x+y)+q(x)+q(y)+q(0)$ の階数を返す。階数は常に偶数である。

### `is_balanced`

```cpp
bool is_balanced() const
```

$q=0$ と $q=1$ の代入数が等しいかを返す。これはある $\gamma_j$ が 1 で
あることと同値である。

### `arf_invariant`

```cpp
optional<bool> arf_invariant() const
```

二次形式がradical上で0なら、非退化商上のArf不変量
$\sum_i\alpha_i\beta_i$ を返す。radical上で非零なら `nullopt` を返す。
定数項はArf不変量へ含めない。

### `evaluate_canonical`

```cpp
bool evaluate_canonical(const vector<bool>& assignment) const
```

標準形の変数順で与えた代入における値を返す。

### `to_original_assignment`

```cpp
vector<bool> to_original_assignment(
    const vector<bool>& canonical_assignment
) const
```

標準形の代入を、同じ値を与える元の変数の代入へ変換する。

### `solution_count`

```cpp
BigInteger solution_count(bool value) const
```

$q(x)=\mathtt{value}$ となる代入数を任意精度整数で返す。

## 時間計算量

変数数を $N$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築 | $O(N^2/64)$ | $O(N^2/64)$ |
| 係数の参照・更新 | $O(1)$ | $O(1)$ |
| `evaluate` | $O(N^2)$ | $O(1)$ |
| `canonicalize` | $O(N^3/64+N^2)$ | $O(N^2/64)$ |
| `bilinear_rank`, `is_balanced`, `arf_invariant`, `evaluate_canonical` | $O(N)$ | $O(1)$ |
| `to_original_assignment` | $O(N^2/64+N)$ | $O(N/64)$ |
| `solution_count` | $O(N)$ bit操作 | $O(N)$ bit |

## 注意点

- 変数番号は $[0,N)$ である。範囲外なら `out_of_range` を送出する。
- 代入の長さが $N$ でなければ `invalid_argument` を送出する。
- `QuadraticFormGF2Canonical` の公開フィールドを書き換えると、各補助関数の
  前提が失われる。
- Boostは使用しない。解数には同梱の `BigInteger` を用いる。
