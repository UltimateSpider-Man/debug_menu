#pragma once

#include "trace.h"

#include <cstdint>

template<typename T>
struct simple_list {
    T *_first_element;
    T *_last_element;
    uint32_t m_size;

    simple_list() : _first_element(nullptr),
                    _last_element(nullptr),
                    m_size(0) {}

    struct vars_t {
        T *_sl_next_element;
        T *_sl_prev_element;
        simple_list<T> *_sl_list_owner;
        
        vars_t() : _sl_next_element(nullptr),
                    _sl_prev_element(nullptr),
                    _sl_list_owner(nullptr) {}
    };

    struct iterator {
        T *_Ptr {nullptr};

        bool operator==(const iterator &it) const {
            return this->_Ptr == it._Ptr;
        }

        bool operator!=(const iterator &it) const {
            return this->_Ptr != it._Ptr;
        }

        void operator++() {
            if ( this->_Ptr != nullptr ) {
                this->_Ptr = this->_Ptr->simple_list_vars._sl_next_element;
            }
        }

        T &operator*() {
            return (*this->_Ptr);
        }

    };

    uint32_t size() const {
        return this->m_size;
    }

    bool empty() const {
        return (this->m_size == 0);
    }

    iterator begin() {
        return iterator {this->_first_element};
    }
    
    iterator end() {
        return iterator {nullptr};
    }

    iterator push_back(T *tmp)
    {
        TRACE("simple_list::push_back");

        assert(tmp != nullptr);

        assert(tmp->simple_list_vars._sl_next_element == nullptr);

        assert(tmp->simple_list_vars._sl_prev_element == nullptr);

        assert(tmp->simple_list_vars._sl_list_owner == nullptr);

        tmp->simple_list_vars._sl_next_element = this->_first_element;
        tmp->simple_list_vars._sl_prev_element = nullptr;

        if ( this->_first_element != nullptr ) {
            this->_first_element->simple_list_vars._sl_prev_element = tmp;
        }

        this->_first_element = tmp;
        if ( tmp->simple_list_vars._sl_next_element == nullptr ) {
            this->_last_element = tmp;
        }

        tmp->simple_list_vars._sl_list_owner = this;

        ++this->m_size;

        iterator a2{tmp};
        return a2;
    }

    iterator emplace_back(T *tmp)
    {
        TRACE("simple_list::emplace_back");

        assert(tmp != nullptr);

        assert(tmp->simple_list_vars._sl_next_element == nullptr);
        
        assert(tmp->simple_list_vars._sl_prev_element == nullptr);

        assert(tmp->simple_list_vars._sl_list_owner == nullptr);

        if ( this->_last_element != nullptr )
        {
            assert(_last_element->simple_list_vars._sl_next_element == nullptr);

            this->_last_element->simple_list_vars._sl_next_element = tmp;
            tmp->simple_list_vars._sl_prev_element = this->_last_element;
            tmp->simple_list_vars._sl_next_element = nullptr;
            this->_last_element = tmp;
            tmp->simple_list_vars._sl_list_owner = this;
            ++this->m_size;
            return iterator {tmp};
        }
        else
        {
            return this->push_back(tmp);
        }
    }

    bool contains(T *iter) const {
        return (iter != nullptr)
                && (iter->simple_list_vars._sl_list_owner == this);
    }

    T * erase(T *iter, bool a3)
    {
        T *result = nullptr;
        if ( iter != nullptr )
        {
            assert(this->contains(iter));

            result = (a3
                        ? iter->simple_list_vars._sl_prev_element
                        : iter->simple_list_vars._sl_next_element
                        );

            auto *sl_prev_element = iter->simple_list_vars._sl_prev_element;
            if ( sl_prev_element != nullptr ) {
                sl_prev_element->simple_list_vars._sl_next_element = iter->simple_list_vars._sl_next_element;
            } else {
                assert(iter->simple_list_vars._sl_list_owner->_first_element == iter);

                iter->simple_list_vars._sl_list_owner->_first_element = iter->simple_list_vars._sl_next_element;
            }

            if ( iter->simple_list_vars._sl_next_element != nullptr ) {
                iter->simple_list_vars._sl_next_element->simple_list_vars._sl_prev_element = iter->simple_list_vars._sl_prev_element;
            } else {
                assert(iter->simple_list_vars._sl_list_owner->_last_element == iter);

                iter->simple_list_vars._sl_list_owner->_last_element = iter->simple_list_vars._sl_prev_element;
            }

            assert(iter->simple_list_vars._sl_list_owner->m_size >= 0);

            --iter->simple_list_vars._sl_list_owner->m_size;
            iter->simple_list_vars._sl_next_element = nullptr;
            iter->simple_list_vars._sl_prev_element = nullptr;
            iter->simple_list_vars._sl_list_owner = nullptr;
        }

        return result;
    }

    bool common_erase(T *a2)
    {
        if ( !this->contains(a2) ) {
            return false;
        }

        this->erase(a2, false);
        return true;
    }

    iterator erase(T *a3)
    {
        auto *v3 = this->erase(a3, false);
        return {v3};
    }

};
