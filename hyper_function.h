#pragma once

#if USE_STDFUNCTION

#include <functional>
namespace hr {
    using std::function;
} // namespace hr

#else

#include <type_traits>
#include <utility>

namespace hr {

template<class Sig>
class hyper_function;

template<class Sig>
using function = hyper_function<Sig>;

template<class R, class... Args>
struct hyper_function_state_base {
    virtual R call(Args...) const = 0;
    virtual hyper_function_state_base *clone() const = 0;
    virtual ~hyper_function_state_base() = default;
};

template<class T, class R, class... Args>
struct hyper_function_state : hyper_function_state_base<R, Args...> {
    using Self = hyper_function_state<T, R, Args...>;
    T t_;
    explicit hyper_function_state(T t) : t_(std::move(t)) {}
    R call(Args... args) const override {
        return const_cast<T&>(t_)(static_cast<Args&&>(args)...);
    }
    hyper_function_state_base<R, Args...> *clone() const override {
        return new Self(*this);
    }
};

template<class R, class... Args>
class hyper_function<R(Args...)>
{
    hyper_function_state_base<R, Args...> *ptr_ = nullptr;
public:
    hyper_function() = default;

    template<class Callable, class = decltype(R(std::declval<typename std::decay<Callable>::type>()(std::declval<Args>()...)))>
    hyper_function(Callable&& t) :
        ptr_(new hyper_function_state<typename std::decay<Callable>::type, R, Args...>(static_cast<Callable&&>(t)))
    {}

    ~hyper_function() {
        delete ptr_;
    }

    hyper_function(hyper_function& rhs) : ptr_(rhs.ptr_ ? rhs.ptr_->clone() : nullptr) {}
    hyper_function(const hyper_function& rhs) : ptr_(rhs.ptr_ ? rhs.ptr_->clone() : nullptr) {}
    hyper_function(hyper_function&& rhs) noexcept : ptr_(rhs.ptr_) { rhs.ptr_ = nullptr; }
    hyper_function(const hyper_function&& rhs) = delete;

    void operator=(hyper_function rhs) noexcept {
        std::swap(ptr_, rhs.ptr_);
    }

    R operator()(Args... args) const {
        return ptr_->call(static_cast<Args&&>(args)...);
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }
};

} // namespace hr

#endif
