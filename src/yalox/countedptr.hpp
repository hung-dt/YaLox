#pragma once

/*---------------------------------------------------------------------------*/

/** counted_ptr - simple reference counted pointer without the atomic overhead
 * of std::shared_ptr.
 *
 * DO NOT USE THIS FOR MULTI-THREADED APP. USE std::shared_ptr INSTEAD.
 */

template <typename T>
class counted_ptr
{
public:
  counted_ptr(T* p = nullptr)
    : ptr_(nullptr)
  {
    if ( p ) ptr_ = new pointer(p);
  }

  counted_ptr(const counted_ptr& other)
  {
    acquire(other.ptr_);
  }

  ~counted_ptr()
  {
    release();
  }

  counted_ptr& operator=(const counted_ptr& other)
  {
    if ( this != &other ) {
      release();
      acquire(other.ptr_);
    }
    return *this;
  }

  T* operator->() const
  {
    return ptr_->p;
  }

  T& operator*() const
  {
    return *(ptr_->p);
  }

  T* get() const
  {
    return ptr_ ? ptr_->p : nullptr;
  }

  explicit operator bool() const
  {
    return ptr_ != nullptr;
  }

  unsigned use_count() const
  {
    return ptr_ ? ptr_->count : 0;
  }

private:
  struct pointer
  {
    pointer(T* p = nullptr, unsigned count = 1)
      : p{ p }
      , count{ count }
    {
    }

    T* p;
    unsigned count;
  }* ptr_;

  inline void acquire(pointer* ptr)
  {
    ptr_ = ptr;
    if ( ptr ) ++(ptr->count);
  }

  inline void release()
  {
    if ( ptr_ ) {
      if ( --(ptr_->count) == 0 ) {
        delete ptr_->p;
        delete ptr_;
      }
      ptr_ = nullptr;
    }
  }
};
