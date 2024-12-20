/*************************************************************************************
 * NOTE: The coroutine transformation you've enabled is a hand coded transformation! *
 *       Most of it is _not_ present in the AST. What you see is an approximation.   *
 *************************************************************************************/
#include <iostream>
#include <coroutine>

using namespace std;

struct resumable_thing
{
  struct promise_type
  {
    inline resumable_thing get_return_object()
    {
      return resumable_thing(resumable_thing(std::coroutine_handle<promise_type>::from_promise(*this)));
    }

    inline std::suspend_never initial_suspend()
    {
      return std::suspend_never{};
    }

    inline std::suspend_never final_suspend() noexcept
    {
      return std::suspend_never{};
    }

    inline void return_void()
    {
    }

    inline void unhandled_exception()
    {
    }

    // inline constexpr promise_type() noexcept = default;
  };

  std::coroutine_handle<promise_type> _coroutine;
  inline constexpr resumable_thing() /* noexcept */ = default;
  // inline resumable_thing(const resumable_thing &) = delete;
  // inline resumable_thing & operator=(const resumable_thing &) = delete;
  inline resumable_thing(resumable_thing &&other)
      : _coroutine{std::coroutine_handle<promise_type>(other._coroutine)}
  {
    other._coroutine.operator=(nullptr);
  }

  inline resumable_thing &operator=(resumable_thing &&other)
  {
    if (&other != this)
    {
      this->_coroutine.operator=(other._coroutine);
      other._coroutine.operator=(nullptr);
    }
  }

  inline explicit resumable_thing(std::coroutine_handle<promise_type> coroutine)
      : _coroutine{std::coroutine_handle<promise_type>(coroutine)}
  {
  }

  inline ~resumable_thing() noexcept
  {
    if (static_cast<bool>(this->_coroutine.operator bool()))
    {
      this->_coroutine.destroy();
    }
  }

  inline void resume()
  {
    this->_coroutine.resume();
  }
};

struct __counterFrame
{
  void (*resume_fn)(__counterFrame *);
  void (*destroy_fn)(__counterFrame *);
  std::__coroutine_traits_impl<resumable_thing>::promise_type __promise;
  int __suspend_index;
  bool __initial_await_suspend_called;
  unsigned int i;
  std::suspend_never __suspend_44_17;
  std::suspend_always __suspend_48_14;
  std::suspend_never __suspend_44_17_1;
};

resumable_thing counter()
{
  /* Allocate the frame including the promise */
  __counterFrame *__f = reinterpret_cast<__counterFrame *>(operator new(__builtin_coro_size()));
  __f->__suspend_index = 0;
  __f->__initial_await_suspend_called = false;

  /* Construct the promise. */
  new (&__f->__promise) std::__coroutine_traits_impl<resumable_thing>::promise_type{};

  resumable_thing __coro_gro = __f->__promise.get_return_object() /* NRVO variable */;

  /* Forward declare the resume and destroy function. */
  void __counterResume(__counterFrame * __f);
  void __counterDestroy(__counterFrame * __f);

  /* Assign the resume and destroy function pointers. */
  __f->resume_fn = &__counterResume;
  __f->destroy_fn = &__counterDestroy;

  /* Call the made up function with the coroutine body for initial suspend.
     This function will be called subsequently by coroutine_handle<>::resume()
     which calls __builtin_coro_resume(__handle_) */
  __counterResume(__f);

  return __coro_gro;
}

/* This function invoked by coroutine_handle<>::resume() */
void __counterResume(__counterFrame *__f)
{
  try
  {
    /* Create a switch to get to the correct resume point */
    switch (__f->__suspend_index)
    {
    case 0:
      break;
    case 1:
      goto __resume_counter_1;
    case 2:
      goto __resume_counter_2;
    }

    /* co_await insights.cpp:44 */
    __f->__suspend_44_17 = __f->__promise.initial_suspend();
    if (!__f->__suspend_44_17.await_ready())
    {
      __f->__suspend_44_17.await_suspend(std::coroutine_handle<resumable_thing::promise_type>::from_address(static_cast<void *>(__f)).operator coroutine_handle());
      __f->__suspend_index = 1;
      __f->__initial_await_suspend_called = true;
      return;
    }

  __resume_counter_1:
    __f->__suspend_44_17.await_resume();
    std::operator<<(std::cout, "counter: called\n");
    for (__f->i = 1;; __f->i++)
    {

      /* co_await insights.cpp:48 */
      __f->__suspend_48_14 = std::suspend_always{};
      if (!__f->__suspend_48_14.await_ready())
      {
        __f->__suspend_48_14.await_suspend(std::coroutine_handle<resumable_thing::promise_type>::from_address(static_cast<void *>(__f)).operator coroutine_handle());
        __f->__suspend_index = 2;
        return;
      }

    __resume_counter_2:
      __f->__suspend_48_14.await_resume();
      std::operator<<(std::operator<<(std::cout, "counter:: resumed (#").operator<<(__f->i), ")\n");
    }

    goto __final_suspend;
  }
  catch (...)
  {
    if (!__f->__initial_await_suspend_called)
    {
      throw;
    }

    __f->__promise.unhandled_exception();
  }

__final_suspend:

  /* co_await insights.cpp:44 */
  __f->__suspend_44_17_1 = __f->__promise.final_suspend();
  if (!__f->__suspend_44_17_1.await_ready())
  {
    __f->__suspend_44_17_1.await_suspend(std::coroutine_handle<resumable_thing::promise_type>::from_address(static_cast<void *>(__f)).operator coroutine_handle());
  }

  ;
}

/* This function invoked by coroutine_handle<>::destroy() */
void __counterDestroy(__counterFrame *__f)
{
  /* destroy all variables with dtors */
  __f->~__counterFrame();
  /* Deallocating the coroutine frame */
  operator delete(__builtin_coro_free(static_cast<void *>(__f)));
}

int main()
{
  std::operator<<(std::cout, "main:    calling counter\n");
  resumable_thing the_counter = counter();
  std::operator<<(std::cout, "main:    resuming counter\n");
  the_counter.resume();
  the_counter.resume();
  the_counter.resume();
  the_counter.resume();
  the_counter.resume();
  std::operator<<(std::cout, "main:    done\n");
  return 0;
}