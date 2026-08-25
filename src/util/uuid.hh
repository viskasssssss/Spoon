#pragma once

#include <cstdint>

#include "def.hh"

namespace spoon {

	class uuid
	{
	public:
		uuid();
		uuid(uint64_t uuid);
		uuid(const uuid&) = default;

		operator uint64_t() const { return m_uuid; }

        uuid& operator=(uuid other) {
            this->m_uuid = other.m_uuid;
            return *this;
        }

	private:
		uint64_t m_uuid;
	};

}

namespace std {
	template <typename T> struct hash;

	template<>
	struct hash<spoon::uuid>
	{
		std::size_t operator()(const spoon::uuid& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}