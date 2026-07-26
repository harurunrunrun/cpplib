---
title: Quaternion3 (3次元クォータニオン)
documentation_of: ../../../../src/algorithm/geometry/3d/quaternion3.hpp
---

`Quaternion3` 型と内積関数をまとめて読み込む互換aggregatorです。

## API

- `Quaternion3`: 四則演算、共役、ノルム、正規化、逆元を提供します。
- `quaternion3_dot(first, second)`: 4成分の内積を返します。

## API別の時間計算量・空間計算量

すべて時間・追加空間ともに $O(1)$ です。

## 注意点

従来の一括include用headerです。型だけが必要なら `quaternion3_type.hpp`、内積だけが必要なら `quaternion3_dot.hpp` を直接includeできます。入力は有限値でなければなりません。