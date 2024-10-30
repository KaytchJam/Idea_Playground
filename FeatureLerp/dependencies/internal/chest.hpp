#pragma once

#include <iostream>
#include <functional>

// Exception thrown when a Chest is opened when it does not have stuff
struct NoStuffException : public std::exception {
private:
    std::string err_msg;
public:
    NoStuffException(std::string& err_message) : err_msg(err_message) {}
    const char* what() {
        return this->err_msg.c_str();
    }
};

// Exception thrown when a sign is a expected when there is stuff
struct HasStuffException : public std::exception {
public:
    HasStuffException() {}
    const char* what() {
        return "The chest has stuff.";
    }
};

// Error handling data type that either has stuff or has a sign with a message
// if items are taken from the chest when it has a sign an error is thrown
template<typename T>
class Chest {
private:
    union ChestContent {
        T stuff;
        std::string message;

        ChestContent() {
            new (&this->message) std::string("");
        }

        ChestContent(const T& item) { 
            new (&this->stuff) T(item); 
        }

        ~ChestContent() {}
    };

    bool has_stuff;
    ChestContent content;

    Chest(T item) : has_stuff(true) {
        new(&this->content) T(item);
    }

    Chest(const T& item, bool s) : has_stuff(s) {
        new(&this->content) T(item);
    }

    Chest(T&& item, bool s) : content(std::move(item)), has_stuff(true) {}

    Chest(const std::string& message, bool hstuff, int filler) : has_stuff(hstuff) {
        new(&this->content) std::string(message);
    }

    Chest(std::string&& message, bool hstuff, int filler) : content(std::move(message)), has_stuff(false) {}

public:
    Chest(const Chest<T>& rhs);
    Chest(Chest<T>&& rhs);

    // Put stuff in the chest
    // static Chest<T> stuff(T item) {
    //     return Chest<T>(item, true);
    // }

    static Chest<T> stuff(T&& item) {
        return Chest<T>(item, true);
    }

    // Put a sign in the chest
    static Chest<T> sign(const std::string& message) {
        return Chest<T>(message, false, 0);
    }

    static Chest<T> sign(std::string&& message) {
        return Chest<T>(message, false, 0);
    }

    ~Chest() {}

    /// Returns whether the chest has stuff.
    bool is_stuffed();

    /// Returns whether the chest has stuff and if the stuff matches some predicate.
    bool is_stuffed_and(std::function<bool(const T&)>);

    /// Returns whether the chest is signed.
    bool is_signed();

    /// Returns whether the chest is signed and if the sign matches some predicate.
    bool is_signed_and(std::function<bool(const std::string&)>);

    /// Open the chest, throws a NoStuffException if the chest has no stuff.
    T& open();

    /// Open the chest and get the sign, throws a HasStuffException if the chest has stuff
    std::string& open_sign() {
        if (!this->has_stuff) {
            return this->content.message;
        }

        throw HasStuffException();
    }

    /// If the chest has stuff, applies the passed in the function on stuff T and maps it to stuff F, returning Chest<F>
    /// In the case that the chest has no stuff, it still returns Chest<F> and the sign is simply copied over
    template<typename F>
    Chest<F> map_stuff(std::function<F(const T&)> func) {
        if (this->has_stuff) {
            return Chest<F>::stuff(func(this->content.stuff));
        }

        return Chest<F>::sign(std::move(this->content.message));
    }

    /// If the chest has stuff returns default, otherwise applies a function on the stuff in the chest and returns it
    template<typename F>
    F map_or(const F& base, std::function<F(const T&)> func) {
        return this->has_stuff ? func(this->content.stuff) : base;
    }

    /// Maps on the stuff in the chest and returns it with func, otherwise calls fallback function default and returns that
    template<typename F>
    F map_or_fallback(std::function<F(const std::string&)> base, std::function<F(const T&)> func) {
        return this->has_stuff ? func(this->content.stuff) : base(this->content.message);
    }

    /// Executes fallback if there is no stuff in the chest and just a sign, otherwise returns the stuff in the chest
    Chest<T> map_sign(std::function<std::string (const std::string&)> fallback) {
        return this->has_stuff ? *this : Chest<T>::sign(fallback(this->content.message));
    }

    /// Appends string message to the current sign if there is no stuff in the chest, otherwise returns a chest with the same stuff
    Chest<T> sign_append(const std::string& new_message) {
        return this->has_stuff ? *this : Chest<T>::sign(this->content.message + new_message);
    }

    /// Appends a newline to the current sign and appends message to it if there is not stuff in the chest, otherwise returns a new Chest
    /// with the same stuff
    Chest<T> sign_append_newline(const std::string& new_message) {
        return this->has_stuff ? *this : Chest<T>::sign(this->content.message + '\n' + new_message);
    }

    // Returns a new chest with a pointer to the contents rather than a copy
    Chest<T*> as_ptr() {
        return this->has_stuff ? Chest<T>::stuff(&this->content.stuff) : Chest<T>::sign(this->content.sign);
    }

    /// Open the chest for the stuff in it, if there is no stuff return the alternative
    T& open_or_get(const T& alternative);

    /// Open the chest for the stuff in it, if there
    // some_function_i_forgot() {}

    /// Expect stuff to be in a chest, if there is no stuff a NoStuffException
    /// is thrown and the failure message is printed.
    T& expect(const std::string& failure_msg);

    /// Expect the chest to be signed. If there is in fact stuff in the chest
    /// a HasStuffException is thrown and the stuff is printed.
    std::string& expect_sign(const std::string& failure_msg);
};

template<typename T>
bool Chest<T>::is_stuffed() {
    return this->has_stuff;
}

template<typename T>
bool Chest<T>::is_stuffed_and(std::function<bool(const T&)> pred) {
    return this->has_stuff && pred(this->content.stuff);
}

template<typename T>
bool Chest<T>::is_signed() {
    return !this->has_stuff;
}

template<typename T>
bool Chest<T>::is_signed_and(std::function<bool(const std::string&)> pred) {
    return !this->has_stuff && pred(this->content.message);
}

template<typename T>
T& Chest<T>::open() {

    if (this->has_stuff) {
        //std::cout << typeid(this->content.stuff).name() << std::endl;
        return this->content.stuff;
    } 

    std::cout << this->content.message << std::endl;
    std::cout << "Throwing exception." << std::endl;
    throw NoStuffException(this->content.message);
}

template<typename T>
T& Chest<T>::open_or_get(const T& alternative) {
    return this->has_stuff ? this->content.stuff : alternative;
}

template<typename T>
T& Chest<T>::expect(const std::string& failure_msg) {
    if (this->has_stuff) {
        return this->content.stuff;
    }

    std::cout << failure_msg << std::endl;
    throw NoStuffException(this->content.message);
}

template<typename T>
std::string& Chest<T>::expect_sign(const std::string& failure_msg) {
    if (!this->has_stuff) {
        return this->content.message;
    }

    std::cout << failure_msg << std::endl;
    throw HasStuffException();
}

template<typename T>
Chest<T>::Chest(const Chest<T>& rhs) : has_stuff(rhs.has_stuff) {
    if (rhs.has_stuff) {
        new (&this->content) T(rhs.content.stuff);
    } else {
        new (&this->content) std::string(rhs.content.message);
    }
}

template<typename T>
Chest<T>::Chest(Chest<T>&& rhs) : has_stuff(rhs.has_stuff) {
    std::cout << "Moving: " << rhs.content.stuff << std::endl;

    if (rhs.has_stuff) {
        new (&this->content) T(std::move(rhs.content.stuff));
    } else {
        new (&this->content) std::string(std::move(rhs.content.message));
    }
}

// template<typename T>
// Chest<T>& Chest<T>::operator=(const Chest<T>& rhs) {
//     return Chest<T>(rhs);
// }

// template<typename T>
// Chest<T> Chest<T>::operator=(Chest<T>&& rhs) {
//     return Chest<T>(rhs);
// }