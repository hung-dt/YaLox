#define ANKERL_NANOBENCH_IMPLEMENT

#include "nanobench.h"
#include "countedptr.hpp"

#include <memory>

class MyClass
{
public:
  std::string desc;
};

int main()
{
  counted_ptr<MyClass> cptr{ new MyClass{
    "This is MyClass managed by counted_ptr" } };

  ankerl::nanobench::Bench().minEpochIterations(2'000'000).run(
    "counted_ptr copy-ctor", [&] {
      auto cp{ cptr };
      ankerl::nanobench::doNotOptimizeAway(cp);
    });

  ankerl::nanobench::Bench().minEpochIterations(2'000'000).run(
    "counted_ptr copy-assign", [&] {
      counted_ptr<MyClass> cp;
      cp = cptr;
      ankerl::nanobench::doNotOptimizeAway(cp);
    });

  auto sptr =
    std::make_shared<MyClass>("This is MyClass managed by shared_ptr.");

  ankerl::nanobench::Bench().minEpochIterations(2'000'000).run(
    "shared_ptr copy-ctor", [&] {
      auto sp{ sptr };
      ankerl::nanobench::doNotOptimizeAway(sp);
    });

  ankerl::nanobench::Bench().minEpochIterations(2'000'000).run(
    "shared_ptr copy-assign", [&] {
      std::shared_ptr<MyClass> sp;
      sp = sptr;
      ankerl::nanobench::doNotOptimizeAway(sp);
    });

  auto rptr = new MyClass{ "This is MyClass managed by raw pointer." };

  ankerl::nanobench::Bench().minEpochIterations(2'000'000).run(
    "raw ptr init", [&] {
      auto rp{ rptr };
      ankerl::nanobench::doNotOptimizeAway(rp);
    });

  ankerl::nanobench::Bench().minEpochIterations(2'000'000).run(
    "raw ptr copy", [&] {
      MyClass* rp;
      rp = rptr;
      ankerl::nanobench::doNotOptimizeAway(rp);
    });

  delete rptr;
  return 0;
}
