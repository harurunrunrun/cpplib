#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SMALL_PRIME_STIRLING_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SMALL_PRIME_STIRLING_NUMBERS_HPP_INCLUDED

// Large-index formulas adapted from the upstream project:
// enumerative_combinatorics/stirling_number_of_the_second_kind_small_p_large_n/
// sol/stirling_number_query.hpp, revision 04c8de378bab67be926325de2871f0babb8e6451.
// Copyright upstream contributors. SPDX-License-Identifier: Apache-2.0.
// Full license text appears at the end of this header.
// Changes: compact triangular
// tables, factorial Lucas queries, unsigned first-kind and surjection queries, validation.

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

class SmallPrimeStirlingNumbers{
    int prime_;
    std::vector<int> factorial_, inverse_factorial_;
    std::vector<std::uint16_t> first_, second_;

    static std::size_t index(int n, int k){
        return static_cast<std::size_t>(n) * (n + 1) / 2 + k;
    }

    int power(int value, int exponent) const{
        int result = 1;
        while(exponent){
            if(exponent & 1) result = result * value % prime_;
            value = value * value % prime_;
            exponent >>= 1;
        }
        return result;
    }

public:
    explicit SmallPrimeStirlingNumbers(int prime, bool first_kind = true, bool second_kind = true)
        : prime_(prime){
        if(prime < 2 || prime > 5000) throw std::invalid_argument("SmallPrimeStirlingNumbers: require prime in [2, 5000]");
        for(int divisor = 2; divisor * divisor <= prime; ++divisor){
            if(prime % divisor == 0) throw std::invalid_argument("SmallPrimeStirlingNumbers: modulus must be prime");
        }
        factorial_.assign(static_cast<std::size_t>(prime), 1);
        inverse_factorial_.assign(static_cast<std::size_t>(prime), 1);
        for(int i = 1; i < prime; ++i) factorial_[i] = factorial_[i - 1] * i % prime;
        inverse_factorial_[prime - 1] = power(factorial_[prime - 1], prime - 2);
        for(int i = prime - 1; i > 0; --i) inverse_factorial_[i - 1] = inverse_factorial_[i] * i % prime;
        const std::size_t table_size = static_cast<std::size_t>(prime) * (prime + 1) / 2;
        if(first_kind){
            first_.resize(table_size);
            first_[0] = 1;
            for(int n = 1; n < prime; ++n){
                for(int k = 0; k <= n; ++k){
                    int value = k ? first_[index(n - 1, k - 1)] : 0;
                    if(k < n) value -= (n - 1) * first_[index(n - 1, k)] % prime;
                    if(value < 0) value += prime;
                    first_[index(n, k)] = static_cast<std::uint16_t>(value);
                }
            }
        }
        if(second_kind){
            second_.resize(table_size);
            second_[0] = 1;
            for(int n = 1; n < prime; ++n){
                for(int k = 0; k <= n; ++k){
                    int value = k ? second_[index(n - 1, k - 1)] : 0;
                    if(k < n) value += k * second_[index(n - 1, k)];
                    second_[index(n, k)] = static_cast<std::uint16_t>(value % prime);
                }
            }
        }
    }

    int binomial(long long n, long long k) const{
        if(k < 0 || n < k) return 0;
        int result = 1;
        while(n){
            const int row = static_cast<int>(n % prime_);
            const int column = static_cast<int>(k % prime_);
            if(column > row) return 0;
            result = result * factorial_[row] % prime_;
            result = result * inverse_factorial_[column] % prime_;
            result = result * inverse_factorial_[row - column] % prime_;
            n /= prime_;
            k /= prime_;
        }
        return result;
    }

    int signed_first(long long n, long long k) const{
        if(first_.empty()) throw std::logic_error("SmallPrimeStirlingNumbers: first-kind table not built");
        if(k < 0 || n < k) return 0;
        const long long quotient = n / prime_;
        const int remainder = static_cast<int>(n % prime_);
        if(quotient > k) return 0;
        long long blocks = (k - quotient) / (prime_ - 1);
        int degree = static_cast<int>((k - quotient) % (prime_ - 1));
        if(degree == 0 && remainder > 0){
            degree += prime_ - 1;
            --blocks;
        }
        if(blocks < 0 || blocks > quotient || degree > remainder) return 0;
        int result = binomial(quotient, blocks) * first_[index(remainder, degree)] % prime_;
        if(((quotient + blocks) & 1) && result) result = prime_ - result;
        return result;
    }

    int unsigned_first(long long n, long long k) const{
        if(k < 0 || n < k) return 0;
        int result = signed_first(n, k);
        if(((n - k) & 1) && result) result = prime_ - result;
        return result;
    }

    int second(long long n, long long k) const{
        if(second_.empty()) throw std::logic_error("SmallPrimeStirlingNumbers: second-kind table not built");
        if(k < 0 || n < k) return 0;
        if(n < prime_) return second_[index(static_cast<int>(n), static_cast<int>(k))];
        const long long quotient = k / prime_;
        const int remainder = static_cast<int>(k % prime_);
        long long blocks = (n - quotient) / (prime_ - 1);
        int degree = static_cast<int>((n - quotient) % (prime_ - 1));
        if(degree == 0){
            degree += prime_ - 1;
            --blocks;
        }
        if(blocks < 0 || remainder > degree) return 0;
        if(degree == prime_ - 1 && remainder == 0) return binomial(blocks, quotient - 1);
        return binomial(blocks, quotient) * second_[index(degree, remainder)] % prime_;
    }

    int surjections(long long n, long long k) const{
        if(k < 0 || n < k || k >= prime_) return 0;
        return factorial_[static_cast<std::size_t>(k)] * second(n, k) % prime_;
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

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_SMALL_PRIME_STIRLING_NUMBERS_HPP_INCLUDED
