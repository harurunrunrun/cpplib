---
title: Angle Interval Set (円周角区間集合)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/angle_interval_set.hpp
---

角度を $2\pi$ 周期で扱い、円周上の半開区間の和集合を保持する。
内部では $[0, 2\pi)$ 上の互いに交わらない線形区間を `std::map` で管理する。

## コンストラクタ

```cpp
AngleIntervalSet()
```

空の集合を構築する。

## 更新

```cpp
void add(long double start, long double end)
void remove(long double start, long double end)
void clear()
```

- `add`: `start` から `end` へ反時計回りに進む半開区間を追加する。
- `remove`: 同じ規約の区間を削除する。
- `clear`: すべての区間を削除する。

## クエリ

```cpp
bool contains(long double angle) const
bool covers(long double start, long double end) const
long double covered_length() const
std::size_t interval_count() const
bool empty() const
bool full() const
std::vector<std::pair<long double, long double>> intervals() const
```

- `contains`: 角度が和集合に含まれるかを返す。各区間の左端を含み、右端を含まない。
- `covers`: 指定した反時計回りの半開区間がすべて覆われているかを返す。空区間に対しては常に `true`。
- `covered_length`: 覆われた角度の総和を $0$ 以上 $2\pi$ 以下で返す。
- `interval_count`: 内部で保持する正規化済みの線形区間の本数を返す。角度 $0$ をまたぐ 1 つの円周区間は 2 本として数えられることがある。
- `empty`, `full`: それぞれ空集合か、全周を覆っているかを返す。
- `intervals`: $[0, 2\pi]$ の範囲に分割した、互いに交わらない半開区間を左端の昇順で返す。

## 時間計算量

$M$ を更新前の線形区間数、$K$ を更新でマージまたは分割される区間数とする。

- `add`, `remove`: $O((K+1) \log (M+1))$
- `contains`, `covers`: $O(\log (M+1))$
- `covered_length`, `interval_count`, `empty`, `full`: $O(1)$
- `intervals`: $O(M)$
- `clear`: $O(M)$

## 空間計算量

保持する区間に $O(M)$、`intervals` の戻り値に $O(M)$。

## 注意点

- 端点はラジアンで指定する。有限値でなければ `std::invalid_argument` を送出する。
- $\lvert \mathit{end} - \mathit{start} \rvert$ が $2\pi - \varepsilon$ 以上なら全周、正規化後の長さが $\varepsilon$ 以下なら空区間として扱う（$\varepsilon$ は `GEOMETRY_EPS`）。
- 数値安定性のため、幅 $\varepsilon$ 以下の隙間はマージする。
- 端点を含むかどうかが重要な離散的な問題では、半開区間の規約に変換してから使う。
