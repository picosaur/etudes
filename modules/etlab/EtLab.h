#pragma once
#include <functional>
#include <ostream>

namespace EtLab {

// Data
// ----------------------------------------------------------------------------
class Data {
  const std::type_info *typeInfo_{};
  std::function<void(void *)> deleter_{};
  void *value_{};

  void reset() {
    typeInfo_ = {};
    deleter_ = {};
    value_ = {};
  }

public:
  Data() {}

  ~Data() {
    if (typeInfo_ != nullptr) {
      deleter_(value_);
      value_ = {};
    }
  }

  Data(Data &&other) {
    typeInfo_ = other.typeInfo_;
    deleter_ = other.deleter_;
    value_ = other.value_;
    other.reset();
  }

  // restrict copy
  Data(const Data &other) = delete;
  Data &operator=(const Data &other) = delete;

  // deafult deleter
  template <typename T> static void CastDelete(void *p) {
    delete static_cast<T *>(p);
  }

  // any-type constructor
  template <typename T, typename CleanT = std::decay_t<T>,
            std::enable_if_t<!std::is_same_v<CleanT, Data>, bool> = true>
  Data(T &&value,
       std::function<void(void *)> deleter = &Data::CastDelete<CleanT>) {
    typeInfo_ = &typeid(value);
    deleter_ = deleter;
    value_ = new CleanT(std::forward<T>(value));
  }

  // type info
  const std::type_info *typeInfo() const { return typeInfo_; }

  // pointer
  template <typename T> T *ptr() const { return static_cast<T *>(value_); }
};

// Var
// ----------------------------------------------------------------------------
class Var {
  Data *data_{};
  void initEmptyData();
  void initData(Data &&data);
  Var varOperator(const char *op, const Var &a);

  friend std::ostream &operator<<(std::ostream &os, const Var &var);

public:
  Var();
  ~Var();

  Var(const Var &arg);
  void operator=(const Var &arg);

  template <typename T, typename CleanT = std::decay_t<T>,
            std::enable_if_t<!std::is_same_v<CleanT, Var>, bool> = true>
  Var(T &&other) {
    initData(Data{std::forward<T>(other)});
  }

  template <typename T> const T &value() {
    if (&typeid(T) == data_->typeInfo()) {
      return (*data_->ptr<T>());
    }
    throw(std::runtime_error("Var::value unexpected conversion"));
  }

  template <typename T> Var operator+(const T &a) {
    return varOperator("+", a);
  }
};

std::ostream &operator<<(std::ostream &os, const Var &var);

// Functions
// ----------------------------------------------------------------------------
using Args = std::vector<Data *>;
using TypeList = std::initializer_list<const std::type_info *>;
typedef Data (*OpFunc)(const Args &args);

void SetContext(const char *ctx);
void SetOperatorFunc(OpFunc &func, const char *op, const TypeList &types);

// Test
// ----------------------------------------------------------------------------
void Test();

} // namespace EtLab
