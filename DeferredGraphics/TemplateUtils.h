/*-------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TemplateUtils.h
Purpose: Managing template component vectors
Language: C++ and Visual Studios 2019
Platform: <VS 2019 16.2, 8gb RAM, 130 GB hard disk space, video card suporting 1280 x 720, Windows 10 64bit>
Project: amir.azmi_CS350_1
Author: Amir Azmi, amr.azmi, 180002217
Creation date: January 4th , 2020
--------------------------------------------------------*/
#pragma once
#include <cstddef>
#include <vector>
#include "MeshComponent.h"
#include "LightComponent.h"
#include "EngineTypes.h"
//fill in all the component classes here

template<typename... Types>
struct TypeList
{
  template<template<typename...> typename T>
  using apply = T<Types...>;

  static constexpr auto size = sizeof...(Types);
};

using ComponentTypeList = TypeList<MeshComponentPtr, LightComponentPtr>; //fill in component types here

namespace detail
{
  template<class T, std::size_t N, class... Args>
  struct get_index_of_element_from_tuple_by_type_impl;

  template<class T, std::size_t N, class... Args>
  struct get_index_of_element_from_tuple_by_type_impl<T, N, T, Args...>
  {
    static constexpr auto value = N;
  };

  template<class T, std::size_t N, class U, class... Args>
  struct get_index_of_element_from_tuple_by_type_impl<T, N, U, Args...>
  {
    static constexpr auto value = get_index_of_element_from_tuple_by_type_impl<T, N + 1, Args...>::value;
  };

  template<class... Trest>
  struct unique_types;

  template<class T1, class T2, class... Trest>
  struct unique_types<T1, T2, Trest...> : private unique_types<T1, T2>
    , unique_types<T1, Trest...>
    , unique_types<T2, Trest...>
  {
  };

  template<class T1, class T2>
  struct unique_types<T1, T2>
  {
    static_assert(!std::is_same<T1, T2>::value, "All types must be unique in this arg list.");
  };

  /*!
     *  @brief
     *    One element is guaranteed to be unique.
     */
  template<class T1>
  struct unique_types<T1>
  {
  };
}  // namespace detail

template<template<typename...> class TContainer, typename... Args>
class ContainerTuple : private detail::unique_types<Args...>
{
private:
  using ContainerTupleImpl = std::tuple<TContainer<Args>...>;

private:
  ContainerTupleImpl m_Impl;

public:
  inline ContainerTuple(void) noexcept :
    m_Impl()
  {
  }

  inline ContainerTupleImpl& raw() noexcept
  {
    return m_Impl;
  }

  inline const ContainerTupleImpl& raw() const noexcept
  {
    return m_Impl;
  }

  template<typename T>
  inline TContainer<T>& get(void) noexcept
  {
    return std::get<detail::get_index_of_element_from_tuple_by_type_impl<T, 0, Args...>::value>(raw());
  }

  template<typename T>
  inline const TContainer<T>& get(void) const noexcept
  {
    return std::get<detail::get_index_of_element_from_tuple_by_type_impl<T, 0, Args...>::value>(raw());
  }
};

template<typename... Args>
using VectorTuple = ContainerTuple<std::vector, Args...>; //make a vector for each of the types
using ComponentLists = ComponentTypeList::apply<VectorTuple>; //create the component vectors