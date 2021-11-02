#pragma once
#include <algorithm>
#include "reflect.hpp"

namespace fastbuffers
{
	template<typename _Ty, typename Func, std::size_t... I>
	constexpr void for_each_impl(_Ty&& val, Func func, std::index_sequence<I...>)
	{
		return (func(reflect::rf_element<I>(std::forward<_Ty>(val))), ...);
	}

	template<typename _Ty, typename Func, std::size_t N = rf_size_v<_Ty>, typename Indices = std::make_index_sequence<N>>
	constexpr void for_each(_Ty&& val, Func func)
	{
		return for_each_impl(std::forward<_Ty>(val), func, Indices{});
	}
}