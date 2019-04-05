#pragma once

#include <type_traits>
#include <utility>

namespace hr {

template<class Sig>
class function;

template<class R, class... Args>
struct function_state_base {
    virtual R call(Args...) const = 0;
    virtual function_state_base *clone() const = 0;
    virtual ~function_state_base() {}
};

template<class T, class R, class... Args>
struct function_state : function_state_base<R, Args...> {
    T t_;
    explicit function_state(T t) : t_(std::move(t)) {}
    R call(Args... args) const /*override*/ {
        return const_cast<T&>(t_)(static_cast<Args&&>(args)...);
    }
    function_state_base<R, Args...> *clone() const /*override*/ {
        return new function_state(*this);
    }
};

template<class R, class... Args>
class function<R(Args...)>
{
    function_state_base<R, Args...> *ptr_;
public:
    function() : ptr_(nullptr) {}

    template<class Callable, class = decltype(R(std::declval<typename std::decay<Callable>::type>()(std::declval<Args>()...)))>
    function(Callable&& t) :
        ptr_(new function_state<typename std::decay<Callable>::type, R, Args...>(static_cast<Callable&&>(t)))
    {}

    ~function() {
        delete ptr_;
    }

    function(function& rhs) : ptr_(rhs.ptr_ ? rhs.ptr_->clone() : nullptr) {}
    function(const function& rhs) : ptr_(rhs.ptr_ ? rhs.ptr_->clone() : nullptr) {}
    function(function&& rhs) noexcept : ptr_(rhs.ptr_) { rhs.ptr_ = nullptr; }
    function(const function&& rhs) = delete;

    void operator=(function rhs) noexcept {
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
