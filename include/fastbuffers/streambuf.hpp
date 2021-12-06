#pragma once
#include <array>
#include <algorithm>
#include <streambuf>

namespace fastbuffers
{
	template<std::size_t N, typename T>
	class stream_buf : public std::basic_streambuf<T,std::char_traits<T>>
	{
		using base_type = std::basic_streambuf<T, std::char_traits<T>>;

	public:
		using iterator = std::array<T,N>::iterator;
		using const_iterator = const iterator;
		using value_type = T;
		using size_type = std::size_t;
		using reference = T&;
		using const_reference = const T&;
		using pointer = T*;
		using const_pointer = const pointer;

		stream_buf()
			: buffer_()
		{
			reset();
		}

		stream_buf(std::span<T> data)
			: stream_buf()
		{
			for (auto iter = data.begin(); iter != data.end(); iter++)
			{
				push_back(*iter);
			}
		}

		stream_buf(const stream_buf& buf)
			: stream_buf(std::span<T>(buf.data(),buf.size()))
		{

		}

		virtual ~stream_buf() = default;

	public:
		value_type& operator[](size_type pos)
		{
			return buffer_.at(pos);
		}

	public:
		std::size_t size() noexcept
		{
			return base_type::pptr() - base_type::gptr();
		}

		std::size_t size() const noexcept
		{
			return base_type::pptr() - base_type::gptr();
		}

		constexpr std::size_t max_size() noexcept
		{
			return buffer_.max_size();
		}

		pointer data() noexcept
		{
			return base_type::gptr();
		}

		pointer data() const noexcept
		{
			return base_type::gptr();
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
		
		void clear() noexcept
		{
			std::fill(buffer_.begin(), buffer_.end(), char{});

			reset();
		}

		template<typename _Ty>
		void push_back(_Ty&& value)
		{
			size() == 0 ? clear() : void();

			constexpr std::size_t sz = sizeof(_Ty);

			std::memcpy(base_type::pptr(), &value, sz);

			commit(sz);
		}

		void commit(std::size_t n) noexcept
		{
			n = std::min<std::size_t>(n, base_type::epptr() - base_type::pptr());
			base_type::pbump(static_cast<int>(n));
			base_type::setg(base_type::eback(), base_type::gptr(), base_type::pptr());
		}

		void consume(std::size_t n) noexcept
		{
			if(base_type::egptr() < base_type::pptr())
				base_type::setg(&buffer_[0], base_type::gptr(), base_type::pptr());

			if(base_type::gptr() + n > base_type::pptr())
				n = base_type::pptr() - base_type::gptr();

			base_type::gbump(static_cast<int>(n));
		}

		template<typename _Ty>
		_Ty pop_front()
		{
			_Ty element{};
			constexpr auto I = sizeof(_Ty);

			std::memcpy(&element, reinterpret_cast<_Ty*>(data()), I);

			consume(I);

			return element;
		}

	private:
		void reset()
		{
			base_type::setg(&buffer_[0], &buffer_[0], &buffer_[0]);
			base_type::setp(&buffer_[0], &buffer_[0] + buffer_.max_size());
		}

	private:
		std::array<T,N> buffer_;
	};
}
