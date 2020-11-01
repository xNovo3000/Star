#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace Star {

	class Packet {

		public:
			Packet();
			Packet(uint32_t);
			~Packet() = default;

			Packet(const Packet&) = default;
			Packet& operator=(const Packet&) = default;

			Packet(Packet&&) noexcept = default;
			Packet& operator=(Packet&&) noexcept = default;

			void append(const void*, uint32_t);
			void read(void*, uint32_t) const;

			Packet& operator<<(const uint32_t);
			Packet& operator<<(const int32_t);
			Packet& operator<<(const uint16_t);
			Packet& operator<<(const int16_t);
			Packet& operator<<(const uint8_t);
			Packet& operator<<(const int8_t);
			Packet& operator<<(const float);
			Packet& operator<<(const std::string&);

			const Packet& operator>>(uint32_t&) const;
			const Packet& operator>>(int32_t&) const;
			const Packet& operator>>(uint16_t&) const;
			const Packet& operator>>(int16_t&) const;
			const Packet& operator>>(uint8_t&) const;
			const Packet& operator>>(int8_t&) const;
			const Packet& operator>>(float&) const;
			const Packet& operator>>(std::string&) const;

			void reserve(uint32_t);
			void resize(uint32_t);

			const uint8_t* data() const noexcept;
			uint8_t* data() noexcept;
			uint32_t size() const noexcept;
			bool empty() const noexcept;
		
		private:
			std::vector<uint8_t> m_Data;
			mutable uint32_t m_Offset;

	};

}