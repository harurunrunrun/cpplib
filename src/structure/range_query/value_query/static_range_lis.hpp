#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_STATIC_RANGE_LIS_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_STATIC_RANGE_LIS_HPP_INCLUDED

// Modified from noshi91's seaweed-doubling and unit-Monge multiplication implementation.
// References:
// Tiskin, A. (2008). Semi-local string comparison: Algorithmic techniques
// and applications. Mathematics in Computer Science, 1(4), 571-603.
// Claude, F., Navarro, G., & Ordónez, A. (2015). The wavelet matrix: An
// efficient wavelet tree for large alphabets. Information Systems, 47, 15-32.
// https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/data_structure/static_range_lis_query/sol/correct.cpp
// SPDX-License-Identifier: Apache-2.0; full license text appears below.
// Changes: reuse the library wavelet matrix, accept repeated arbitrary keys,
// provide both strict and weak increasing subsequences, and validate intervals.

#include <algorithm>
#include <bit>
#include <cstddef>
#include "../../array/static_or_vector.hpp"
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../wavelet_matrix/static/wavelet_matrix.hpp"

namespace static_range_lis_internal{
using vi = std::vector<int>;
using iter = vi::iterator;
inline constexpr int none = -1;

inline vi inverse(const vi &p) {
  const int n = p.size();
  vi q(n, none);
  for (int i = 0; i < n; i++) {
    if (p[i] != none) {
      q[p[i]] = i;
    }
  }
  return q;
}

inline void unit_monge_dmul(const int n, iter stack, const iter a, const iter b) {
  if (n == 1) {
    stack[0] = 0;
    return;
  }
  const iter c_row = stack;
  stack += n;
  const iter c_col = stack;
  stack += n;

  const auto map = [=](const int len, const auto f, const auto g) {
    const iter a_h = stack;
    const iter a_m = stack + len;
    const iter b_h = stack + std::ptrdiff_t(2) * len;
    const iter b_m = stack + std::ptrdiff_t(3) * len;
    const auto split = [=](const iter v, iter v_h, iter v_m) {
      for (int i = 0; i < n; i++) {
        if (f(v[i])) {
          *v_h = g(v[i]);
          ++v_h;
          *v_m = i;
          ++v_m;
        }
      }
    };
    split(a, a_h, a_m);
    split(b, b_h, b_m);
    const iter c = stack + std::ptrdiff_t(4) * len;
    unit_monge_dmul(len, c, a_h, b_h);
    for (int i = 0; i < len; i++) {
      const int row = a_m[i];
      const int col = b_m[c[i]];
      c_row[row] = col;
      c_col[col] = row;
    }
  };
  const int mid = n / 2;
  map(mid, [mid](const int x) { return x < mid; },
      [](const int x) { return x; });
  map(n - mid, [mid](const int x) { return x >= mid; },
      [mid](const int x) { return x - mid; });

  class d_itr {
  public:
    int delta;
    int col;
    d_itr() : delta(0), col(0) {}
  };
  int row = n;
  const auto right = [&](d_itr &it) {
    if (b[it.col] < mid) {
      if (c_col[it.col] >= row) {
        it.delta += 1;
      }
    } else {
      if (c_col[it.col] < row) {
        it.delta += 1;
      }
    }
    it.col += 1;
  };
  const auto up = [&](d_itr &it) {
    if (a[row] < mid) {
      if (c_row[row] >= it.col) {
        it.delta -= 1;
      }
    } else {
      if (c_row[row] < it.col) {
        it.delta -= 1;
      }
    }
  };
  d_itr neg, pos;
  while (row != 0) {
    while (pos.col != n) {
      d_itr temp = pos;
      right(temp);
      if (temp.delta == 0) {
        pos = temp;
      } else {
        break;
      }
    }
    row -= 1;
    up(neg);
    up(pos);
    while (neg.delta != 0) {
      right(neg);
    }
    if (neg.col > pos.col) {
      c_row[row] = pos.col;
    }
  }
}

inline vi subunit_monge_dmul(vi a, vi b) {
  const int n = a.size();
  vi a_inv = inverse(a);
  vi b_inv = inverse(b);
  std::swap(b, b_inv);
  vi a_map, b_map;
  for (int i = n - 1; i >= 0; i--) {
    if (a[i] != none) {
      a_map.push_back(i);
      a[n - a_map.size()] = a[i];
    }
  }
  std::reverse(a_map.begin(), a_map.end());
  {
    int cnt = 0;
    for (int i = 0; i < n; i++) {
      if (a_inv[i] == none) {
        a[cnt] = i;
        cnt += 1;
      }
    }
  }
  for (int i = 0; i < n; i++) {
    if (b[i] != none) {
      b[b_map.size()] = b[i];
      b_map.push_back(i);
    }
  }
  {
    int cnt = b_map.size();
    for (int i = 0; i < n; i++) {
      if (b_inv[i] == none) {
        b[cnt] = i;
        cnt += 1;
      }
    }
  }
  vi c([](int n) {
    std::size_t ret = 0;
    while (n > 1) {
      ret += std::size_t(2) * n;
      n = n / 2 + n % 2;
      ret += std::size_t(4) * n;
    }
    ret += 1;
    return ret;
  }(n));
  unit_monge_dmul(n, c.begin(), a.begin(), b.begin());

  vi c_pad(n, none);
  for (int i = 0; i < int(a_map.size()); i++) {
    const int t = c[n - a_map.size() + i];
    if (t < int(b_map.size())) {
      c_pad[a_map[i]] = b_map[t];
    }
  }
  return c_pad;
}

inline vi seaweed_doubling(const vi &p) {
  const int n = p.size();
  if (n == 1) {
    return vi({none});
  }
  const int mid = n / 2;
  vi lo, hi;
  vi lo_map, hi_map;
  for (int i = 0; i < n; i++) {
    const int e = p[i];
    if (e < mid) {
      lo.push_back(e);
      lo_map.push_back(i);
    } else {
      hi.push_back(e - mid);
      hi_map.push_back(i);
    }
  }
  lo = seaweed_doubling(lo);
  hi = seaweed_doubling(hi);
  vi lo_pad(n), hi_pad(n);
  std::iota(lo_pad.begin(), lo_pad.end(), 0);
  std::iota(hi_pad.begin(), hi_pad.end(), 0);
  for (int i = 0; i < mid; i++) {
    if (lo[i] == none) {
      lo_pad[lo_map[i]] = none;
    } else {
      lo_pad[lo_map[i]] = lo_map[lo[i]];
    }
  }
  for (int i = 0; mid + i < n; i++) {
    if (hi[i] == none) {
      hi_pad[hi_map[i]] = none;
    } else {
      hi_pad[hi_map[i]] = hi_map[hi[i]];
    }
  }
  return subunit_monge_dmul(std::move(lo_pad), std::move(hi_pad));
}

} // namespace static_range_lis_internal

template<class T, int MAX_SIZE = -1>
class StaticRangeLIS{
    static_assert(MAX_SIZE == -1 || MAX_SIZE > 0);
    int n;
    WaveletMatrix<unsigned, MAX_SIZE, std::bit_width(static_cast<unsigned>(MAX_SIZE))> matrix;
    static std::vector<unsigned> prepare(const std::vector<T>& a, bool nondecreasing){
        if(MAX_SIZE >= 0 && a.size() > static_cast<std::size_t>(MAX_SIZE)) throw std::out_of_range("static range LIS capacity");
        const int n = cpplib_detail::checked_int_size(a.size());
        std::vector<int> order(n), permutation(n);
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int x, int y){
            if(a[x] < a[y]) return true;
            if(a[y] < a[x]) return false;
            return nondecreasing ? x < y : x > y;
        });
        for(int i = 0; i < n; ++i) permutation[order[i]] = i;
        std::vector<int> row;
        if(n) row = static_range_lis_internal::seaweed_doubling(permutation);
        std::vector<unsigned> result(n);
        for(int i = 0; i < n; ++i) result[i] = row[i] == -1 ? n : row[i];
        return result;
    }
public:
    explicit StaticRangeLIS(const std::vector<T>& a, bool nondecreasing = false):
        n(cpplib_detail::checked_int_size(a.size())), matrix(prepare(a, nondecreasing)){}
    int size() const{ return n; }
    int query(int l, int r) const{
        if(l < 0 || l > r || r > n) throw std::out_of_range("static range LIS interval");
        return r - l - matrix.range_freq(l, n, static_cast<unsigned>(r));
    }
};


/*
Third-party license for the adapted portions of this header.

                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   1. Definitions.

      "License" shall mean the terms and conditions for use, reproduction,
      and distribution as defined by Sections 1 through 9 of this document.

      "Licensor" shall mean the copyright owner or entity authorized by
      the copyright owner that is granting the License.

      "Legal Entity" shall mean the union of the acting entity and all
      other entities that control, are controlled by, or are under common
      control with that entity. For the purposes of this definition,
      "control" means (i) the power, direct or indirect, to cause the
      direction or management of such entity, whether by contract or
      otherwise, or (ii) ownership of fifty percent (50%) or more of the
      outstanding shares, or (iii) beneficial ownership of such entity.

      "You" (or "Your") shall mean an individual or Legal Entity
      exercising permissions granted by this License.

      "Source" form shall mean the preferred form for making modifications,
      including but not limited to software source code, documentation
      source, and configuration files.

      "Object" form shall mean any form resulting from mechanical
      transformation or translation of a Source form, including but
      not limited to compiled object code, generated documentation,
      and conversions to other media types.

      "Work" shall mean the work of authorship, whether in Source or
      Object form, made available under the License, as indicated by a
      copyright notice that is included in or attached to the work
      (an example is provided in the Appendix below).

      "Derivative Works" shall mean any work, whether in Source or Object
      form, that is based on (or derived from) the Work and for which the
      editorial revisions, annotations, elaborations, or other modifications
      represent, as a whole, an original work of authorship. For the purposes
      of this License, Derivative Works shall not include works that remain
      separable from, or merely link (or bind by name) to the interfaces of,
      the Work and Derivative Works thereof.

      "Contribution" shall mean any work of authorship, including
      the original version of the Work and any modifications or additions
      to that Work or Derivative Works thereof, that is intentionally
      submitted to Licensor for inclusion in the Work by the copyright owner
      or by an individual or Legal Entity authorized to submit on behalf of
      the copyright owner. For the purposes of this definition, "submitted"
      means any form of electronic, verbal, or written communication sent
      to the Licensor or its representatives, including but not limited to
      communication on electronic mailing lists, source code control systems,
      and issue tracking systems that are managed by, or on behalf of, the
      Licensor for the purpose of discussing and improving the Work, but
      excluding communication that is conspicuously marked or otherwise
      designated in writing by the copyright owner as "Not a Contribution."

      "Contributor" shall mean Licensor and any individual or Legal Entity
      on behalf of whom a Contribution has been received by Licensor and
      subsequently incorporated within the Work.

   2. Grant of Copyright License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      copyright license to reproduce, prepare Derivative Works of,
      publicly display, publicly perform, sublicense, and distribute the
      Work and such Derivative Works in Source or Object form.

   3. Grant of Patent License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      (except as stated in this section) patent license to make, have made,
      use, offer to sell, sell, import, and otherwise transfer the Work,
      where such license applies only to those patent claims licensable
      by such Contributor that are necessarily infringed by their
      Contribution(s) alone or by combination of their Contribution(s)
      with the Work to which such Contribution(s) was submitted. If You
      institute patent litigation against any entity (including a
      cross-claim or counterclaim in a lawsuit) alleging that the Work
      or a Contribution incorporated within the Work constitutes direct
      or contributory patent infringement, then any patent licenses
      granted to You under this License for that Work shall terminate
      as of the date such litigation is filed.

   4. Redistribution. You may reproduce and distribute copies of the
      Work or Derivative Works thereof in any medium, with or without
      modifications, and in Source or Object form, provided that You
      meet the following conditions:

      (a) You must give any other recipients of the Work or
          Derivative Works a copy of this License; and

      (b) You must cause any modified files to carry prominent notices
          stating that You changed the files; and

      (c) You must retain, in the Source form of any Derivative Works
          that You distribute, all copyright, patent, trademark, and
          attribution notices from the Source form of the Work,
          excluding those notices that do not pertain to any part of
          the Derivative Works; and

      (d) If the Work includes a "NOTICE" text file as part of its
          distribution, then any Derivative Works that You distribute must
          include a readable copy of the attribution notices contained
          within such NOTICE file, excluding those notices that do not
          pertain to any part of the Derivative Works, in at least one
          of the following places: within a NOTICE text file distributed
          as part of the Derivative Works; within the Source form or
          documentation, if provided along with the Derivative Works; or,
          within a display generated by the Derivative Works, if and
          wherever such third-party notices normally appear. The contents
          of the NOTICE file are for informational purposes only and
          do not modify the License. You may add Your own attribution
          notices within Derivative Works that You distribute, alongside
          or as an addendum to the NOTICE text from the Work, provided
          that such additional attribution notices cannot be construed
          as modifying the License.

      You may add Your own copyright statement to Your modifications and
      may provide additional or different license terms and conditions
      for use, reproduction, or distribution of Your modifications, or
      for any such Derivative Works as a whole, provided Your use,
      reproduction, and distribution of the Work otherwise complies with
      the conditions stated in this License.

   5. Submission of Contributions. Unless You explicitly state otherwise,
      any Contribution intentionally submitted for inclusion in the Work
      by You to the Licensor shall be under the terms and conditions of
      this License, without any additional terms or conditions.
      Notwithstanding the above, nothing herein shall supersede or modify
      the terms of any separate license agreement you may have executed
      with Licensor regarding such Contributions.

   6. Trademarks. This License does not grant permission to use the trade
      names, trademarks, service marks, or product names of the Licensor,
      except as required for reasonable and customary use in describing the
      origin of the Work and reproducing the content of the NOTICE file.

   7. Disclaimer of Warranty. Unless required by applicable law or
      agreed to in writing, Licensor provides the Work (and each
      Contributor provides its Contributions) on an "AS IS" BASIS,
      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
      implied, including, without limitation, any warranties or conditions
      of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
      PARTICULAR PURPOSE. You are solely responsible for determining the
      appropriateness of using or redistributing the Work and assume any
      risks associated with Your exercise of permissions under this License.

   8. Limitation of Liability. In no event and under no legal theory,
      whether in tort (including negligence), contract, or otherwise,
      unless required by applicable law (such as deliberate and grossly
      negligent acts) or agreed to in writing, shall any Contributor be
      liable to You for damages, including any direct, indirect, special,
      incidental, or consequential damages of any character arising as a
      result of this License or out of the use or inability to use the
      Work (including but not limited to damages for loss of goodwill,
      work stoppage, computer failure or malfunction, or any and all
      other commercial damages or losses), even if such Contributor
      has been advised of the possibility of such damages.

   9. Accepting Warranty or Additional Liability. While redistributing
      the Work or Derivative Works thereof, You may choose to offer,
      and charge a fee for, acceptance of support, warranty, indemnity,
      or other liability obligations and/or rights consistent with this
      License. However, in accepting such obligations, You may act only
      on Your own behalf and on Your sole responsibility, not on behalf
      of any other Contributor, and only if You agree to indemnify,
      defend, and hold each Contributor harmless for any liability
      incurred by, or claims asserted against, such Contributor by reason
      of your accepting any such warranty or additional liability.

   END OF TERMS AND CONDITIONS

   APPENDIX: How to apply the Apache License to your work.

      To apply the Apache License to your work, attach the following
      boilerplate notice, with the fields enclosed by brackets "[]"
      replaced with your own identifying information. (Don't include
      the brackets!)  The text should be enclosed in the appropriate
      comment syntax for the file format. We also recommend that a
      file or class name and description of purpose be included on the
      same "printed page" as the copyright notice for easier
      identification within third-party archives.

   Copyright 2024 Kohei Morita

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_STATIC_RANGE_LIS_HPP_INCLUDED
