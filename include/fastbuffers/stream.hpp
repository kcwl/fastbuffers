#pragma once
#include "streambuf.hpp"
#include "type_traits.hpp"
#include "algorithm.hpp"

namespace fastbuffers
{
	inline constexpr static std::size_t buffer_capacity = 4 * 1024;

	template<std::size_t Capacity = buffer_capacity>
	class stream
	{
	public:
		stream()
			: buffer_()
		{
		}

		stream(stream_buf<Capacity> ios)
			: buffer_(ios)
		{

		}

	public:
		template<typename _Ty>
		constexpr int push_back(_Ty&& value)
		{
			put(std::forward<_Ty>(value));

			return 1;
		}

		template<typename _Ty>
		auto pop_front() -> _Ty
		{
			return get<_Ty>();
		}



	public:
		template<typename T>
		stream& operator << (T&& value)
		{
			push_back(std::move(value));
			return *this;
		}

		template<typename T>
		stream& operator >> (T& value)
		{
			value = pop_front<T>();
			return *this;
		}

		stream_buf<Capacity>::value_type* data() noexcept
		{
			return buffer_.data();
		}

		stream_buf<Capacity>::value_type* data() const noexcept
		{
			return buffer_.data();
		}

		std::size_t size() noexcept
		{
			return buffer_.size();
		}

		constexpr std::size_t size() const noexcept
		{
			return buffer_.size();
		}

		std::size_t max_size() noexcept
		{
			return buffer_.max_size();
		}

		std::size_t max_size() const noexcept
		{
			return buffer_.max_size();
		}

		void commit(std::size_t n) noexcept
		{
			return buffer_.commit(n);
		}

	private:
		template<typename _Ty>
		constexpr void put(_Ty&& value)
		{
			for_each(std::move(value), [this](auto&& iter)
					 {
						 push(std::move(iter));
					 });
		}

		template<typename _Ty>
		requires(std::is_pod_v<_Ty>)
			constexpr void push(_Ty&& val)
		{
			for_each(std::move(val), [&](auto&& iter)
					 {
						 buffer_.push_back(iter);
					 });
		}

		template<typename _Ty>
		requires(is_container_v<_Ty>)
			void put(_Ty&& val)
		{
			int32_t size = static_cast<int32_t>(val.size());
			push(std::move(size));
			std::for_each(val.begin(), val.end(), [this](auto iter)
						  {
							  put(std::move(iter));
						  });
		}

		template<typename _Ty>
		requires(!std::is_pod_v<_Ty> && !is_container_v<_Ty>)
			void push(_Ty&& val)
		{
			for_each(std::move(val), [&](auto&& iter)
					 {
						 put(iter);
					 });
		}

		template<typename _Ty>
		requires(is_container_v<_Ty>)
			void push(_Ty&& val)
		{
			return put(std::move(val));
		}

		template<typename _Ty>
		auto get()
		{
			return pop<_Ty>();
		}

		template<typename _Ty, std::size_t N = reflect::rf_size_v<_Ty>, typename Indices = std::make_index_sequence<N>>
		_Ty pop()
		{
			return pop_impl<_Ty>(Indices{});
		}

		template<typename _Ty, std::size_t... I>
		auto pop_impl(std::index_sequence<I...>)
		{
			return _Ty{make_element<I,_Ty>()...};
		}

		template<std::size_t I, typename _Ty>
		auto make_element()
		{
			auto element = reflect::rf_element<I>(_Ty{});

			using element_t = decltype(element);

			if constexpr(!std::is_trivial_v<element_t>)
			{
				return get<element_t>();
			}
			else
			{
				constexpr auto N = sizeof(element_t);

				std::memcpy(&element, reinterpret_cast<element_t*>(buffer_.data()), N);

				buffer_.consume(N);

				return element;
			}
		}

		template<typename _Ty>
		requires(is_container_v<_Ty>)
			auto get()
		{
			_Ty element{};

			int32_t element_size{};

			constexpr std::size_t  N = sizeof(int32_t);

			using element_type = _Ty::value_type;

			std::memcpy(&element_size, reinterpret_cast<int32_t*>(buffer_.data()), N);

			buffer_.consume(N);

			for(int32_t i = 0; i != element_size; ++i)
			{
				auto ele = pop<element_type>();

				element.push_back(ele);
			}

			return element;
		}

	private:
		stream_buf<Capacity> buffer_;
	};
}