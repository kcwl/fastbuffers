#pragma once
#include <functional>
#include <tuple>

namespace fastbuffers
{
	template<typename _Ty, typename _ = void>
	struct is_container : std::false_type {};

	template<typename _Ty>
	struct is_container<_Ty, std::void_t<typename _Ty::value_type,
		typename _Ty::size_type,
		typename _Ty::allocator_type,
		typename _Ty::iterator,
		typename _Ty::const_iterator,
		decltype(std::declval<_Ty>().size()),
		decltype(std::declval<_Ty>().begin()),
		decltype(std::declval<_Ty>().end()),
		decltype(std::declval<_Ty>().cbegin()),
		decltype(std::declval<_Ty>().cend())>> : std::true_type{};

	template<typename _Ty>
	constexpr static bool is_container_v = is_container<std::remove_cvref_t<_Ty>>::value;


	template<typename _Ty>
	struct is_pod : std::false_type {};

	template<typename _Ty>
	requires(std::is_standard_layout_v<_Ty> && std::is_trivial_v<_Ty>)
	struct is_pod<_Ty> : std::true_type{};

	template<typename _Ty>
	constexpr static bool is_pod_v = is_pod<std::remove_cvref_t<_Ty>>::value;
}

