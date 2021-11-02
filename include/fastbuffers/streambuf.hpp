#pragma once
#include <array>
#include <algorithm>
#include <streambuf>

namespace fastbuffers
{
	template<std::size_t N, typename T = char>
	class stream_buf : public std::streambuf
	{
	public:
		using iterator = std::array<T,N>::iterator;
		using const_iterator = const iterator;
		using value_type = T;
		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const pointer;

		inline constexpr static std::size_t buffer_capacity = 4 * 1024;

		stream_buf()
			: buffer_()
		{
			setg(&buffer_[0], &buffer_[0], &buffer_[0]);
			setp(&buffer_[0], &buffer_[0] + buffer_.max_size());
		}

		virtual ~stream_buf() = default;

	public:
		constexpr std::size_t size() const noexcept
		{
			return pptr() - gptr();
		}

		std::size_t max_size() noexcept
		{
			return buffer_.max_size();
		}

		constexpr pointer data() noexcept
		{
			return gptr();
		}

		constexpr pointer data() const noexcept
		{
			return gptr();
		}

		constexpr iterator begin() noexcept
		{
			return buffer_.begin();
		}

		constexpr const_iterator begin() const noexcept
		{
			return buffer_.begin();
		}

		constexpr iterator end() noexcept
		{
			return buffer_.end();
		}

		constexpr const_iterator end() const noexcept
		{
			return buffer_.end();
		}

		template<typename _Ty>
		constexpr void push_back(_Ty&& value)
		{
			constexpr std::size_t sz = sizeof(_Ty);
			std::memcpy(pptr(), &value, sz);

			commit(sz);
		}

		constexpr void commit(std::size_t n) noexcept
		{
			n = std::min<std::size_t>(n, epptr() - pptr());
			pbump(static_cast<int>(n));
			setg(eback(), gptr(), pptr());
		}

		void consume(std::size_t n) noexcept
		{
			if(egptr() < pptr())
				setg(&buffer_[0], gptr(), pptr());

			if(gptr() + n > pptr())
				n = pptr() - gptr();

			gbump(static_cast<int>(n));
		}

	private:
		std::array<T,N> buffer_;
	};
}
