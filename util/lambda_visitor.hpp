#ifndef __LAMBDA_VISITOR_HPP
#define __LAMBDA_VISITOR_HPP

#include <variant>
#include <tuple>
#include <optional>
template<class... Ts> struct lambda_visitor : Ts... { using Ts::operator()...; };
template<class... Ts> lambda_visitor(Ts...) -> lambda_visitor<Ts...>;

template <typename Variant, typename... Matchers>
auto match(Variant&& variant, Matchers&&... matchers)
{
    return std::visit(
            lambda_visitor{std::forward<Matchers>(matchers)...},
            std::forward<Variant>(variant));
}
template <typename Variant,typename Variant2, typename... Matchers>
auto match_double(Variant&& variant, Variant2&& variant2,Matchers&&... matchers)
{
    return std::visit(
            lambda_visitor{std::forward<Matchers>(matchers)...},
            std::forward<Variant>(variant),std::forward<Variant2>(variant2));
}
template <typename Base, typename Variant,typename Callable>
auto match_base(Variant&& variant,Callable callable){
    return match(std::forward<Variant>(variant),
            [&callable](auto& arg){
                 if constexpr(std::is_convertible<decltype(arg),Base&>::value){
                    return callable(arg);
                 }
            }
            );
}


template <typename Base, typename Variant,typename Callable>
auto match_exact(Variant&& variant,Callable callable){
    return match(std::forward<Variant>(variant),
            [&callable](Base& arg){
                    return callable(arg);
                 },
            [](auto& ignore){

            }
            );
}


template <typename T, typename Variant>
std::optional<T> extract_value(Variant variant_value) {
    return match(variant_value,
            [](T &value) -> std::optional<T> { return value; },
            [](auto other) -> std::optional<T> { return {}; }
    );
}
template <typename T, typename Variant>
std::optional<std::reference_wrapper<T>> extract_reference(Variant variant_value) {
    return match(variant_value,
            [](T &value) -> std::optional<std::reference_wrapper<T>> { return value; },
            [](auto other) -> std::optional<std::reference_wrapper<T>> { return {}; }
    );
}

template <size_t N, typename... Args>
decltype(auto) magic_get(Args&&... as) noexcept {
    return std::get<N>(std::forward_as_tuple(std::forward<Args>(as)...));
}
#endif
