#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BOUNDED_MONOTONE_SEQUENCES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BOUNDED_MONOTONE_SEQUENCES_HPP_INCLUDED

// Modified from the lattice-path convolution reference implementation:
// Algorithm reference: https://noshi91.hatenablog.com/entry/2023/07/21/235339
// https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/enumerative_combinatorics/number_of_increasing_sequences_between_two_sequences/sol/correct.cpp
// SPDX-License-Identifier: Apache-2.0; full license text appears below.
// Changes: use the existing modular arithmetic and NTT; add signed bounds,
// strict monotonicity, empty inputs, validation, and a function API.

#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include "../../fft/transform/convolution.hpp"

namespace bounded_monotone_sequences_internal{
using std::vector;
using std::min;
using std::max;
using std::reverse;
using std::upper_bound;
using std::abs;

struct BoundedMonotonePathEngine{
private:

  using mint = Modint998244353;


  int n;
  vector<int> a,b;
  int zero_flag=0;
  long long mod = 998244353;
  vector<mint> fac,finv,inv;
  // i!, 1/i!, 1/i

  // v = (v_0, v_1, ... , v_{n-1}) に対し
  // f(x) = sum_i ( n-1-i+xCx v_i ) とする
  // {f(0), f(1), ... , f(m-1)} を返す
  vector<mint> enumerate_f(int _n,vector<mint> &v,int m){
    vector<mint> _v=v;
    reverse(_v.begin(),_v.end());
    for(int i=0;i<(_n);++i){
      _v[i]*=finv[i];
    }
    reverse(_v.begin(),_v.end());
    // _v = (v_{0}/(n-1)!, v_{1}/(n-2)!, ... , v_{n-1}/0!)

    vector<mint> fsub(_n+m);
    for(int i=0;i<(_n+m);++i){
      fsub[i]=fac[i];
    }

    vector<mint> f=convolution_ntt(_v,fsub),res;
    for(int i=0;i<(m);++i){
      res.emplace_back(f[_n-1+i]*finv[i]);
    }
    return res;
  }

  // v = (v_0, v_1, ... , v_{n-1}), l に対し
  // g(x) = sum_i ( l-i+xCl v_i ) とする
  // {g(0), g(1), ... , g(n-1)} を返す
  vector<mint> enumerate_g(int _n,vector<mint> &v,int l){
    vector<mint> _v=v;

    vector<mint> gsub(_n-1,0);
    for(int i=0;i<(_n);++i){
      gsub.emplace_back(fac[l+i]*finv[i]);
    }

    vector<mint> g=convolution_ntt(_v,gsub),res;
    for(int i=0;i<(_n);++i){
      res.emplace_back(g[_n-1+i]*finv[l]);
    }
    return res;
  }

  

  // 長さ _n の(広義単調増加な)非負整数列 _a に対し以下を考える
  // 横の長さを _n とする辺がある 左から i の地点から _a[i] だけ上に伸びる辺がある
  // 上手い具合に横線が引いてある
  // また下辺の左から i の地点には start[i] が書かれている
  // この状態でマス目数え上げ DP をしたとき、一番右の辺に書かれた数字 _a[_n-1] 個を返す
  vector<mint> sub(int _n,vector<int> &_a,vector<mint> &start){
    int m=_a[_n-1];
    vector<mint> res(m+1);
    
    if(_n==1){
      for(int i=0;i<(m+1);++i){
        res[i]=start[0];
      }
      return res;
    }
    if(_n==2){
      for(int i=0;i<(m+1);++i){
        res[i]=start[0]*min((int)i+1,_a[0]+1)+start[1];
      }
      return res;
    }

    // _n >= 2
    int mid=_n/2;
    int m_front=_a[mid];
    vector<int> _a_front(mid+1),_a_back(_n-mid);
    vector<mint> start_front(mid+1),start_end(_n-mid,0);
    
    for(int i=0;i<(mid+1);++i){
      _a_front[i]=_a[i];
      start_front[i]=start[i];
      if(i==mid)start_front[i]=0;
    }

    for(int i=mid;i<(_n);++i){
      _a_back[i-mid]=_a[i]-m_front;
      start_end[i-mid]=start[i];
    }

    vector<mint> sub_front=sub(mid+1,_a_front,start_front);
    // sub_front は長さ m_front+1

    for(int i=m_front;i>=(1);--i){
      sub_front[i]-=sub_front[i-1];
    }
    

    vector<mint> sub_front_f=enumerate_f(m_front+1,sub_front,_n-mid);
    vector<mint> sub_front_g=enumerate_g(m_front+1,sub_front,_n-mid-1);

    vector<mint> start_end_f=enumerate_f(_n-mid,start_end,m_front+1);
    vector<mint> start_end_g=enumerate_g(_n-mid,start_end,m_front);

    for(int i=0;i<(m_front);++i){
      res[i]=sub_front_g[i]+start_end_f[i];
    }

    for(int i=0;i<(_n-mid);++i){
      start_end[i]=sub_front_f[i]+start_end_g[i];
    }

    for(int i=_n-mid-1;i>=(1);--i){
      start_end[i]-=start_end[i-1];
    }

    vector<mint> sub_end=sub(_n-mid,_a_back,start_end);


    for(int i=0;i<((int)sub_end.size());++i){
      res[i+m_front]=sub_end[i];
    }

    return res;
  }

public:
  BoundedMonotonePathEngine() = default;
  BoundedMonotonePathEngine(int _n,vector<int> _a,vector<int> _b){
    n=_n;
    a=_a;
    b=_b;

    for(int i=0;i<(n);++i){
      if(a[i]>b[i])zero_flag=1;
      if(i>0&&b[i]<a[i-1])zero_flag=1;
    }

    for(int i=n-2;i>=(0);--i){
      b[i]=min(b[i],b[i+1]);
    }

    for(int i=1;i<(n);++i){
      a[i]=max(a[i],a[i-1]);
    }

    int al=a[n-1],bl=b[n-1];

    for(int i=n-1;i>=(1);--i){
      a[i]=min(a[i],a[i-1]);
    }

    int dec=a[0];
    for(int i=0;i<(n);++i)a[i]-=dec;
    for(int i=0;i<(n);++i)b[i]-=dec;

    n++;
    a.emplace_back(al-dec);
    b.emplace_back(bl+5-dec);

    for(int i=0;i<(n);++i){
      if(a[i]>b[i])zero_flag=1;
      if(i>0&&b[i]<a[i-1])zero_flag=1;
    }

    int m=max((n+max(a[n-1],b[n-1]))*2+10,100);
    fac.resize(m);
    finv.resize(m);
    inv.resize(m);
    fac[0]=fac[1]=1;
    finv[0]=finv[1]=1;
    inv[1]=1;
    for(int i=2;i<(m);++i){
      fac[i]=fac[i-1]*i;
      inv[i]=-inv[mod%i]*(mod/i);
      finv[i]=finv[i-1]*inv[i];
    }
  }
  
  // 以下を満たす "広義" 単調増加の整数列 x の個数を返す
  // a_i <= x_i <= b_i
  mint answer(){
    if(zero_flag)return 0;
    if(n==1){
      return (mint)b[0]-a[0]+1;
    }
    

    int dist=upper_bound(a.begin(),a.end(),a[0])-a.begin();
    // [0,dist) までは a_i = a_0

    int px=0,py=a[0];
    int qx=dist-1,qy=a[0];
    if(qx==0){
      qy=b[0];
    }

    vector<mint> now(abs(qx-px)+abs(qy-py)+1,0);
    now[0]=1;

    while(qx!=n-1||qy!=b[n-1]){
      // cout<<"# "<<px<<" "<<py<<" "<<qx<<" "<<qy<<endl;
      int sz=now.size();
      for(int i=sz-1;i>=(1);--i){
        if(i==1&&px==0&&py==0)break;
        now[i]-=now[i-1];
      }

      

      if(py==qy){
        // 上に伸ばす
        vector<int> _a(qx-px+1);
        for(int i=0;i<(qx-px+1);++i){
          _a[i]=b[px+i]-py;
          //if(i<qx-px-1)_a[i]++;
        }
        
        now=sub(qx-px+1,_a,now);
        px=qx,py=qy;
        qy=b[qx];
      }
      else{
        // 右に伸ばす
        int index=upper_bound(a.begin(),a.end(),qy)-a.begin();
        // (px,py), (qx=px,qy) ->
        // (qx,qy), (index,qy)
        vector<int> _a(qy-py+1);
        for(int i=0;i<(qy-py+1);++i){
          int index2=upper_bound(a.begin(),a.end(),py+i)-a.begin();
          _a[i]=index2-px;
          //if(i<qy-py-1)_a[i]++;
        }
        for(int i=qy-py;i>=(0);--i)_a[i]-=_a[0];
        
        

        now=sub(qy-py+1,_a,now);
        px=qx,py=qy;
        qx=index-1;
      }
    }

    

    return now[now.size()-1];
  }
};
} // namespace bounded_monotone_sequences_internal

inline Modint998244353 count_bounded_monotone_sequences(
    const std::vector<int>& lower, const std::vector<int>& upper,
    bool strictly_increasing = false){
    if(lower.size() != upper.size()) throw std::invalid_argument("monotone sequence bound sizes");
    const int n = static_cast<int>(lower.size());
    if(n == 0) return 1;
    std::vector<std::int64_t> a(n), b(n);
    for(int i = 0; i < n; ++i){
        if(lower[i] >= upper[i]) return 0;
        a[i] = static_cast<std::int64_t>(lower[i]) - (strictly_increasing ? i : 0);
        b[i] = static_cast<std::int64_t>(upper[i]) - 1 - (strictly_increasing ? i : 0);
    }
    for(int i = 1; i < n; ++i) a[i] = std::max(a[i], a[i-1]);
    for(int i = n-1; i > 0; --i) b[i-1] = std::min(b[i-1], b[i]);
    for(int i = 0; i < n; ++i) if(a[i] > b[i]) return 0;
    const auto shift = a[0];
    const std::int64_t width = b.back() - shift + 1;
    if(n + width > 1'000'000) throw std::length_error("monotone sequence lattice width exceeds capacity");
    std::vector<int> x(n), y(n);
    for(int i = 0; i < n; ++i){ x[i] = static_cast<int>(a[i] - shift); y[i] = static_cast<int>(b[i] - shift); }
    bounded_monotone_sequences_internal::BoundedMonotonePathEngine engine(n, x, y);
    return engine.answer();
}


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

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_BOUNDED_MONOTONE_SEQUENCES_HPP_INCLUDED
