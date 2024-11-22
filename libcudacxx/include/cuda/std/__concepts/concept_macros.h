//===----------------------------------------------------------------------===//
//
// Copyright (c) Facebook, Inc. and its affiliates.
// Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//
// Part of libcu++, the C++ Standard Library for your entire system,
// under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES.
//
//===----------------------------------------------------------------------===//

#ifndef _CUDA___CONCEPTS
#define _CUDA___CONCEPTS

#include <cuda/std/detail/__config>

#if defined(_CCCL_IMPLICIT_SYSTEM_HEADER_GCC)
#  pragma GCC system_header
#elif defined(_CCCL_IMPLICIT_SYSTEM_HEADER_CLANG)
#  pragma clang system_header
#elif defined(_CCCL_IMPLICIT_SYSTEM_HEADER_MSVC)
#  pragma system_header
#endif // no system header

////////////////////////////////////////////////////////////////////////////////
// _CCCL_TEMPLATE
// Usage:
//   _CCCL_TEMPLATE(typename A, typename _Bp)
//     _CCCL_REQUIRES( Concept1<A> _CCCL_AND Concept2<_Bp>)
//   void foo(A a, _Bp b)
//   {}

// Barebones enable if implementation to use outside of cuda::std
template <bool>
struct __cccl_select
{};

template <>
struct __cccl_select<true>
{
  template <class _Tp>
  using type = _Tp;
};

template <bool _Bp, class _Tp = void>
using __cccl_enable_if_t = typename __cccl_select<_Bp>::template type<_Tp>;

template <class _Tp, bool _Bp>
using __cccl_requires_t = typename __cccl_select<_Bp>::template type<_Tp>;

#if (defined(__cpp_concepts) && _CCCL_STD_VER >= 2020)
#  define _CCCL_TEMPLATE(...)               template <__VA_ARGS__>
#  define _CCCL_REQUIRES(...)               requires __VA_ARGS__
#  define _CCCL_AND                         &&
#  define _CCCL_TRAILING_REQUIRES_AUX_(...) requires __VA_ARGS__
#  define _CCCL_TRAILING_REQUIRES(...)      ->__VA_ARGS__ _CCCL_TRAILING_REQUIRES_AUX_
#else // ^^^ __cpp_concepts ^^^ / vvv !__cpp_concepts vvv
#  define _CCCL_TEMPLATE(...)               template <__VA_ARGS__
#  define _CCCL_REQUIRES(...)               , bool _CCCL_true_ = true, __cccl_enable_if_t < __VA_ARGS__ && _CCCL_true_, int > = 0 >
#  define _CCCL_AND                         &&_CCCL_true_, int > = 0, __cccl_enable_if_t <
#  define _CCCL_TRAILING_REQUIRES_AUX_(...) , __VA_ARGS__ >
#  define _CCCL_TRAILING_REQUIRES(...)      ->__cccl_requires_t < __VA_ARGS__ _CCCL_TRAILING_REQUIRES_AUX_
#endif // !__cpp_concepts

#if _CCCL_STD_VER >= 2014

namespace __cccl_concept
{

template <typename...>
struct _Tag;

template <class>
_LIBCUDACXX_HIDE_FROM_ABI constexpr bool __is_true()
{
  return true;
}

#  if _CCCL_COMPILER(CLANG) || defined(_CCCL_COMPILER_MSVC)
template <bool _Bp>
_LIBCUDACXX_HIDE_FROM_ABI __cccl_enable_if_t<_Bp> __cccl_requires()
{}
#  else // ^^^ _CCCL_COMPILER(CLANG) || defined(_CCCL_COMPILER_MSVC) ^^^ / vvv other compilers vvv
template <bool _Bp, __cccl_enable_if_t<_Bp, int> = 0>
_CCCL_INLINE_VAR constexpr int __cccl_requires = 0;
#  endif // !_CCCL_COMPILER(CLANG) && !defined(_CCCL_COMPILER_MSVC)

template <class _Tp, class... _Args>
_LIBCUDACXX_HIDE_FROM_ABI auto __cccl_make_dependent(_Tp*, _Tag<_Args...>*) -> _Tp;

template <class _Impl, class... _Args>
using __requires_expr_impl =
  decltype(__cccl_make_dependent(static_cast<_Impl*>(nullptr), static_cast<_Tag<void, _Args...>*>(nullptr)));

} // namespace __cccl_concept

// So that we can refer to the ::cuda::std namespace below
_LIBCUDACXX_BEGIN_NAMESPACE_STD
_LIBCUDACXX_END_NAMESPACE_STD

// We put an alias for _CUDA_VSTD here because of a bug in nvcc <12.2
// where a requirement such as:
//
//  { expression } -> ::concept<type>
//
// where ::concept is a fully qualified name, would not compile. The
// _CUDA_VSTD macro is fully qualified.
namespace __unqualified_cuda_std = _CUDA_VSTD; // NOLINT(misc-unused-alias-decls)

#  if _CCCL_CUDACC_BELOW(12, 2)
#    define _CCCL_CONCEPT_VSTD __unqualified_cuda_std // must not be fully qualified
#  else
#    define _CCCL_CONCEPT_VSTD _CUDA_VSTD
#  endif

#  define _CCCL_CONCEPT_FRAGMENT_REQS_M0(_REQ) _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_(_REQ)(_REQ)
#  define _CCCL_CONCEPT_FRAGMENT_REQS_M1(_REQ) _CCCL_PP_EXPAND _REQ
#  define _CCCL_CONCEPT_FRAGMENT_REQS_(...)    {_CCCL_PP_FOR_EACH(_CCCL_CONCEPT_FRAGMENT_REQS_M, __VA_ARGS__)}
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_(_REQ)                                                              \
    _CCCL_PP_CAT3(_CCCL_CONCEPT_FRAGMENT_REQS_SELECT_,                                                           \
                  _CCCL_PP_EVAL(_CCCL_PP_CHECK, _CCCL_PP_CAT3(_CCCL_CONCEPT_FRAGMENT_REQS_SELECT_PROBE_, _REQ))) \
    /**/
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_PROBE_requires _CCCL_PP_PROBE_N(~, 1)
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_PROBE_noexcept _CCCL_PP_PROBE_N(~, 2)
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_PROBE_typename _CCCL_PP_PROBE_N(~, 3)
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_PROBE__Same_as _CCCL_PP_PROBE_N(~, 4)

#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_0 _CCCL_PP_EXPAND
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_1 _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_OR_NOEXCEPT
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_2 _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_OR_NOEXCEPT
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_3 _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_OR_NOEXCEPT
#  define _CCCL_CONCEPT_FRAGMENT_REQS_SELECT_4 _CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS
#  define _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_OR_NOEXCEPT(_REQ) \
    _CCCL_PP_CAT4(_CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_, _REQ)
#  define _CCCL_PP_EAT_TYPENAME_PROBE_typename _CCCL_PP_PROBE(~)
#  define _CCCL_PP_EAT_TYPENAME_SELECT_(_Xp, ...) \
    _CCCL_PP_CAT3(_CCCL_PP_EAT_TYPENAME_SELECT_,  \
                  _CCCL_PP_EVAL(_CCCL_PP_CHECK, _CCCL_PP_CAT3(_CCCL_PP_EAT_TYPENAME_PROBE_, _Xp)))
#  define _CCCL_PP_EAT_TYPENAME_(...)                            \
    _CCCL_PP_EVAL2(_CCCL_PP_EAT_TYPENAME_SELECT_, __VA_ARGS__, ) \
    (__VA_ARGS__)
#  define _CCCL_PP_EAT_TYPENAME_SELECT_0(...) __VA_ARGS__
#  define _CCCL_PP_EAT_TYPENAME_SELECT_1(...) _CCCL_PP_CAT3(_CCCL_PP_EAT_TYPENAME_, __VA_ARGS__)
#  define _CCCL_PP_EAT_TYPENAME_typename

#  if (defined(__cpp_concepts) && _CCCL_STD_VER >= 2020) || defined(_CCCL_DOXYGEN_INVOKED)

#    define _CCCL_CONCEPT concept

#    define _CCCL_CONCEPT_FRAGMENT(_NAME, ...)        concept _NAME = _CCCL_PP_CAT(_CCCL_CONCEPT_FRAGMENT_REQS_, __VA_ARGS__)
#    define _CCCL_CONCEPT_FRAGMENT_REQS_requires(...) requires(__VA_ARGS__) _CCCL_CONCEPT_FRAGMENT_REQS_
#    define _CCCL_CONCEPT_FRAGMENT_REQS_M(_REQ)                             \
      _CCCL_PP_CAT2(_CCCL_CONCEPT_FRAGMENT_REQS_M, _CCCL_PP_IS_PAREN(_REQ)) \
      (_REQ);
#    define _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_requires(...) requires __VA_ARGS__
#    define _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_typename(...) typename _CCCL_PP_EAT_TYPENAME_(__VA_ARGS__)
#    define _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_noexcept(...) \
      {                                                        \
        __VA_ARGS__                                            \
      } noexcept
#    define _CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS(_REQ) \
      {                                               \
        _CCCL_PP_CAT4(_CCCL_PP_EAT_SAME_AS_, _REQ)    \
      } -> _CCCL_CONCEPT_VSTD::same_as<_CCCL_PP_EVAL( \
          _CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS_AUX, _CCCL_PP_CAT4(_CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS_, _REQ))>
#    define _CCCL_PP_EAT_SAME_AS__Same_as(...)
#    define _CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS_AUX(_TYPE, ...) _CCCL_PP_EXPAND _TYPE
#    define _CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS__Same_as(...)   (__VA_ARGS__),

#    define _CCCL_FRAGMENT(_NAME, ...) _NAME<__VA_ARGS__>

#  else

#    define _CCCL_CONCEPT _CCCL_INLINE_VAR constexpr bool

#    define _CCCL_CONCEPT_FRAGMENT(_NAME, ...)                                                                         \
      _LIBCUDACXX_HIDE_FROM_ABI auto _NAME##_CCCL_CONCEPT_FRAGMENT_impl_ _CCCL_CONCEPT_FRAGMENT_REQS_##__VA_ARGS__ > { \
      }                                                                                                                \
      template <typename... _As>                                                                                       \
      _LIBCUDACXX_HIDE_FROM_ABI char _NAME##_CCCL_CONCEPT_FRAGMENT_(                                                   \
        __cccl_concept::_Tag<_As...>*, decltype(&_NAME##_CCCL_CONCEPT_FRAGMENT_impl_<_As...>));                        \
      _LIBCUDACXX_HIDE_FROM_ABI char(&_NAME##_CCCL_CONCEPT_FRAGMENT_(...))[2] /**/
#    if defined(_MSC_VER) && !defined(__clang__)
#      define _CCCL_CONCEPT_FRAGMENT_TRUE(...) \
        __cccl_concept::__is_true<decltype(_CCCL_PP_FOR_EACH(_CCCL_CONCEPT_FRAGMENT_REQS_M, __VA_ARGS__) void())>()
#    else
#      define _CCCL_CONCEPT_FRAGMENT_TRUE(...) \
        !(decltype(_CCCL_PP_FOR_EACH(_CCCL_CONCEPT_FRAGMENT_REQS_M, __VA_ARGS__) void(), false){})
#    endif
#    define _CCCL_CONCEPT_FRAGMENT_REQS_requires(...) (__VA_ARGS__)->__cccl_enable_if_t < _CCCL_CONCEPT_FRAGMENT_REQS_2_
#    define _CCCL_CONCEPT_FRAGMENT_REQS_2_(...)       _CCCL_CONCEPT_FRAGMENT_TRUE(__VA_ARGS__)
#    define _CCCL_CONCEPT_FRAGMENT_REQS_M(_REQ)                             \
      _CCCL_PP_CAT2(_CCCL_CONCEPT_FRAGMENT_REQS_M, _CCCL_PP_IS_PAREN(_REQ)) \
      (_REQ),
#    define _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_requires(...) __cccl_concept::__cccl_requires<__VA_ARGS__>
#    define _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_typename(...) static_cast<__cccl_concept::_Tag<__VA_ARGS__>*>(nullptr)
#    if _CCCL_COMPILER(GCC, <, 14)
// GCC < 14 can't mangle noexcept expressions, so just check that the
// expression is well-formed.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70790
#      define _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_noexcept(...) __VA_ARGS__
#    else
#      define _CCCL_CONCEPT_FRAGMENT_REQS_REQUIRES_noexcept(...) __cccl_concept::__cccl_requires<noexcept(__VA_ARGS__)>
#    endif
#    define _CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS(_REQ) \
      __cccl_concept::__cccl_requires<                \
        _CUDA_VSTD::same_as<_CCCL_PP_CAT4(_CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS_, _REQ) _CCCL_PP_RPAREN>>
#    define _CCCL_CONCEPT_FRAGMENT_REQS_SAME_AS__Same_as(...) __VA_ARGS__, decltype _CCCL_PP_LPAREN

#    define _CCCL_FRAGMENT(_NAME, ...) \
      (1u == sizeof(_NAME##_CCCL_CONCEPT_FRAGMENT_(static_cast<__cccl_concept::_Tag<__VA_ARGS__>*>(nullptr), nullptr)))

#  endif

////////////////////////////////////////////////////////////////////////////////
// _CCCL_REQUIRES_EXPR
// Usage:
//   template <typename T>
//   _CCCL_CONCEPT equality_comparable =
//     _CCCL_REQUIRES_EXPR((T), T const& lhs, T const& rhs) (
//       lhs == rhs,
//       lhs != rhs
//     );
//
// Can only be used as the last requirement in a concept definition.
#  if defined(__cpp_concepts) && _CCCL_STD_VER >= 2020
#    define _CCCL_REQUIRES_EXPR(_TY, ...) requires(__VA_ARGS__) _CCCL_REQUIRES_EXPR_2
#    define _CCCL_REQUIRES_EXPR_2(...)    {_CCCL_PP_FOR_EACH(_CCCL_CONCEPT_FRAGMENT_REQS_M, __VA_ARGS__)}
#  else
#    define _CCCL_REQUIRES_EXPR_TPARAM_PROBE_variadic _CCCL_PP_PROBE(~)
#    define _CCCL_REQUIRES_EXPR_TPARAM_variadic

#    define _CCCL_REQUIRES_EXPR_DEF_TPARAM(_TY)       \
      , _CCCL_PP_CAT(_CCCL_REQUIRES_EXPR_DEF_TPARAM_, \
                     _CCCL_PP_EVAL(_CCCL_PP_CHECK, _CCCL_PP_CAT(_CCCL_REQUIRES_EXPR_TPARAM_PROBE_, _TY)))(_TY)
#    define _CCCL_REQUIRES_EXPR_DEF_TPARAM_0(_TY) class _TY
#    define _CCCL_REQUIRES_EXPR_DEF_TPARAM_1(_TY) class... _CCCL_PP_CAT(_CCCL_REQUIRES_EXPR_TPARAM_, _TY)

#    define _CCCL_REQUIRES_EXPR_EXPAND_TPARAM(_TY)       \
      , _CCCL_PP_CAT(_CCCL_REQUIRES_EXPR_EXPAND_TPARAM_, \
                     _CCCL_PP_EVAL(_CCCL_PP_CHECK, _CCCL_PP_CAT(_CCCL_REQUIRES_EXPR_TPARAM_PROBE_, _TY)))(_TY)
#    define _CCCL_REQUIRES_EXPR_EXPAND_TPARAM_0(_TY) _TY
#    define _CCCL_REQUIRES_EXPR_EXPAND_TPARAM_1(_TY) _CCCL_PP_CAT(_CCCL_REQUIRES_EXPR_TPARAM_, _TY)...

#    define _CCCL_REQUIRES_EXPR_TPARAMS(...) _CCCL_PP_FOR_EACH(_CCCL_REQUIRES_EXPR_DEF_TPARAM, __VA_ARGS__)

#    define _CCCL_REQUIRES_EXPR_EXPAND_TPARAMS(...) _CCCL_PP_FOR_EACH(_CCCL_REQUIRES_EXPR_EXPAND_TPARAM, __VA_ARGS__)

#    define _CCCL_REQUIRES_EXPR(_TY, ...)                                                                             \
      __cccl_concept::__requires_expr_impl<struct _CCCL_PP_CAT(                                                       \
        __cccl_requires_expr_detail_, __LINE__) _CCCL_REQUIRES_EXPR_EXPAND_TPARAMS _TY>::                             \
        __cccl_is_satisfied(static_cast<__cccl_concept::_Tag<void _CCCL_REQUIRES_EXPR_EXPAND_TPARAMS _TY>*>(nullptr), \
                            static_cast<void (*)(__VA_ARGS__)>(nullptr));                                             \
      struct _CCCL_PP_CAT(__cccl_requires_expr_detail_, __LINE__)                                                     \
      {                                                                                                               \
        using _Self_t = _CCCL_PP_CAT(__cccl_requires_expr_detail_, __LINE__);                                         \
        template <class _CCCL_REQUIRES_EXPR_TPARAMS _TY>                                                              \
        _LIBCUDACXX_HIDE_FROM_ABI static auto _Well_formed(__VA_ARGS__) _CCCL_REQUIRES_EXPR_2

#    define _CCCL_REQUIRES_EXPR_2(...)                                                                          \
      ->decltype(_CCCL_PP_FOR_EACH(_CCCL_CONCEPT_FRAGMENT_REQS_M, __VA_ARGS__) void()) {}                       \
      template <class... Args, class Sig, class = decltype(static_cast<Sig*>(&_Self_t::_Well_formed<Args...>))> \
      _LIBCUDACXX_HIDE_FROM_ABI static constexpr bool __cccl_is_satisfied(__cccl_concept::_Tag<Args...>*, Sig*) \
      {                                                                                                         \
        return true;                                                                                            \
      }                                                                                                         \
      _LIBCUDACXX_HIDE_FROM_ABI static constexpr bool __cccl_is_satisfied(void*, ...)                           \
      {                                                                                                         \
        return false;                                                                                           \
      }                                                                                                         \
      }
#  endif

#endif // _CCCL_STD_VER >= 2014

#endif //_CUDA___CONCEPTS