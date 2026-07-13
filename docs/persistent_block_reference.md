---
title: Persistent Block Reference (永続ブロック参照)
documentation_of: ../src/structure/wavelet_matrix/detail/persistent_block_reference.hpp
---

永続Wavelet Matrixのblock番号列を管理する固定容量の完全永続区間木。

## API

```cpp
wavelet_matrix_detail::PersistentBlockReference<MAX_BLOCK, MAX_UPDATE> refs;
```

- `build(values,size)`：先頭 `size` 個から初期rootを作り、そのroot番号を返す。空列では `-1`。
- `get(root,position)`：指定rootの `position` 番目を返す。
- `set(root,position,value)`：1点を置換した新rootを返す。
- `nodes_used()`：現在までに使用したnode数を返す。
- `rollback(snapshot)`：使用node数を `snapshot` へ戻す。

## 時間計算量

- `build`：$O(size)$ 時間・$O(size)$ node
- `get`：$O(\log size)$ 時間、$O(1)$ 追加空間計算量
- `set`：$O(\log size)$ 時間・node
- `nodes_used`, `rollback`：$O(1)$

## 注意点

- 永続Wavelet Matrix基底の内部部品であり、通常は直接使用しない。
- `build` は `0 <= size <= MAX_BLOCK` を検査する。`get` / `set` は有効なrootと `[0,size)` の点を要求するが、内部利用を前提として個別には検査しない。
- `rollback` 後に破棄されたrootは無効。node容量またはsnapshot違反では `runtime_error`。
