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
  //struct that contains types, put all my types int T
  template<template<typename...> typename T>
  using apply = T<Types...>;

  //how many types i got in my componentlist
  static constexpr auto size = sizeof...(Types);
};

using ComponentTypeList = TypeList<MeshComponentPtr, LightComponentPtr>; //fill in component types here

namespace detail
{
  //forward declaring index element
  template<class T, std::size_t N, class... Args>
  struct get_index_of_element_from_tuple_by_type_impl;

  //specialization when T is T we reached the end of the loop with some value N
  template<class T, std::size_t N, class... Args>
  struct get_index_of_element_from_tuple_by_type_impl<T, N, T, Args...>
  {
    static constexpr auto value = N;
  };

  //increments index and checks it by seeing if the specialization gets called above
  //T and U where U is the first thing in Args recursively strip off U from the Arg list
  template<class T, std::size_t N, class U, class... Args>
  struct get_index_of_element_from_tuple_by_type_impl<T, N, U, Args...>
  {
    //increment index and check if specialization is hit, if so go uo to the T and T check else strip the U by replacing with
    //the arg list
    static constexpr auto value = get_index_of_element_from_tuple_by_type_impl<T, N + 1, Args...>::value;
  };

  //forward declaring the struct
  template<class... Trest>
  struct unique_types;

  //compile time for loop is the name what it does is it checks every permutation of container of types to make sure they are unique
  template<class T1, class T2, class... Trest>
  struct unique_types<T1, T2, Trest...> : private unique_types<T1, T2>
    , unique_types<T1, Trest...>
    , unique_types<T2, Trest...>
  {
  };

  //Tuples MUST hold unique container of types and this checks for that case
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
  //container can be anything and it gets me the type of that container
  //this tuple gets a list of containers with a specific type
  using ContainerTupleImpl = std::tuple<TContainer<Args>...>;

private:
  ContainerTupleImpl m_Impl;

public:
  inline ContainerTuple(void) noexcept :
    m_Impl()
  {
  }

  //gets the container of the specific type
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
    //what is the index of T
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


//struct that contains the singular type of the pointer
template <typename T>
struct ContainerPointerHolder 
{
  T pointer;
};

template<typename... Args>
using PointerTuple = ContainerTuple<ContainerPointerHolder, Args...>; //tuple of container type ContainerPointerHolder with the arguments of that type
using ComponentPointerLists = ComponentTypeList::apply<PointerTuple>; //create the component pointer lists