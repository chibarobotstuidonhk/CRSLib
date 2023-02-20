#pragma once

#include <type_traits>
#include <memory>

namespace CRSLib
{
	template<class T>
	requires std::is_object_v<T>
	class BoxedObject final
	{
		std::unique_ptr<T> p;

	public:
		BoxedObject(auto&& ... args):
			p{std::make_unique<T>(std::forward<decltype(args)>(args) ...)}
		{}

		BoxedObject(const BoxedObject& other):
			p{other.p ? std::make_unique<T>(*other.p) : nullptr}
		{}

		BoxedObject(BoxedObject&& other) = default;

		operator T&() & noexcept
		{
			return *p;
		}

		operator T&&() && noexcept
		{
			return std::move(*p);
		}
	};
}
