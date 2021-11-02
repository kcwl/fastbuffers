#pragma once
#include "reflect/tuple_reflect.hpp"
#include "reflect/detail.hpp"


namespace fastbuffers
{
#define MACRO(...) __VA_ARGS__

#define REFLECT_DEFINE(...)	MACRO(__VA_ARGS__) MAKE_REFLECT(__VA_ARGS__)

	template<typename T>
	constexpr auto rf_name()
	{
		return reflect::rf_name<T>();
	}

	template<size_t N, typename T>
	constexpr auto rf_element(const T& val)
	{
		return reflect::rf_element<N>(val);
	}

	template<typename T, std::size_t N>
	constexpr auto rf_elem_name() -> std::string_view
	{
		return reflect::rf_elem_name<T,N>();
	}

	template<typename T, std::size_t N>
	using rf_elemet_t = typename reflect::rf_elem_type<T, N>::type;

	template<typename T>
	inline constexpr static std::size_t rf_size_v = reflect::rf_size<T>::value;
}