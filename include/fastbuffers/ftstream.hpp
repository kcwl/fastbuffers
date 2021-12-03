#pragma once
#include <span>
#include "streambuf.hpp"
#include "type_traits.hpp"
#include "algorithm.hpp"

namespace fastbuffers
{
	inline constexpr static std::size_t capacity = 4 * 1024;

	template<std::size_t I = capacity, typename _T = char>
	class ftstream
	{
	public:
		ftstream()
			: buffer_()
		{
		}

		ftstream(stream_buf<I> ios)
			: buffer_(ios)
		{

		}

		ftstream(std::span<_T> data)
		{
			for (auto iter : data)
			{
				push_back(iter);
			}
		}

	public:
		template<typename T>
		ftstream& operator << (T&& value)
		{
			push(std::move(value));
			return *this;
		}

		template<typename T>
		ftstream& operator >> (T& value)
		{
			value = pop<T>();
			return *this;
		}

		ftstream complete(std::size_t length)
		{
			ftstream buf{};

			length > size() ? length = size() : 0;

			for (int32_t i = 4; i < length; i++)
			{
				buf << buffer_[i];
			}

			buffer_.consume(length);

			buffer_ = stream_buf<I,_T>(std::span<char>(buffer_.data(), buffer_.size()));

			return buf;
		}

		void clear()
		{
			return buffer_.clear();
		}

		stream_buf<I, _T>::value_type* data() noexcept
		{
			return buffer_.data();
		}

		stream_buf<I, _T>::value_type* data() const noexcept
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

		void commit(std::size_t n) noexcept
		{
			return buffer_.commit(n);
		}

	private:
		template<typename _Ty>
		requires(std::is_pod_v<std::remove_cvref_t<_Ty>>)
		void push(_Ty&& value)
		{
			for_each(std::move(value), [&](auto&& iter)
					 {
						 buffer_.push_back(iter);
					 });
		}

		template<typename _Ty>
		requires(is_container_v<_Ty>)
		void push(_Ty&& value)
		{
			std::size_t size = value.size();

			push(size);

			for (auto iter : value)
			{
				push(iter);
			}
		}

		template<typename _Ty>
		void push(_Ty&& value)
		{
			for_each(std::move(value), [&](auto&& iter)
				{
					push(iter);
				});
		}

		template<std::size_t I, typename _Ty>
		auto make_element()
		{
			auto element = reflect::rf_element<I>(_Ty{});

			using element_t = decltype(element);

			if constexpr(!std::is_trivial_v<element_t>)
			{
				return pop<element_t>();
			}
			else
			{
				return buffer_.pop_front<element_t>();
			}
		}

		template<typename _Ty, std::size_t... I>
		auto pop_impl(std::index_sequence<I...>)
		{
			return _Ty{ make_element<I,_Ty>()... };
		}

		template<typename _Ty, std::size_t N = fastbuffers::rf_size_v<_Ty>, typename Indices = std::make_index_sequence<N>>
		_Ty pop()
		{
			return pop_impl<_Ty>(Indices{});
		}

		template<typename _Ty>
		requires(is_container_v<_Ty>)
		_Ty pop()
		{
			_Ty element{};

			std::size_t element_size = buffer_.pop_front<std::size_t>();

			using element_type = _Ty::value_type;

			for(int32_t i = 0; i != element_size; ++i)
			{
				auto ele = pop<element_type>();

				element.push_back(ele);
			}

			return element;
		}

	private:
		stream_buf<I, _T> buffer_;
	};
}