#pragma once
#include <cstring>
#include <functional>
#include <ostream>

namespace EtLab {
// Data
// ----------------------------------------------------------------------------
class Data {
  const std::type_info *typeInfo_{};
  std::size_t typeSize_{};
  std::size_t typeAlign_{};
  std::function<void(void *)> deleter_{};
  std::function<void *(void *)> copier_{};
  void *value_{};

  void reset() {
    typeInfo_ = {};
    typeSize_ = {};
    typeAlign_ = {};
    deleter_ = {};
    copier_ = {};
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
    typeSize_ = other.typeSize_;
    typeAlign_ = other.typeAlign_;
    deleter_ = other.deleter_;
    copier_ = other.copier_;
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

  // deafult copier
  template <typename T> static void *CastCopy(void *p) {
    return new T(*static_cast<T *>(p)); // call copy constructor
  }

  // clone method
  Data clone() const {
    Data other;
    other.typeInfo_ = typeInfo_;
    other.typeSize_ = typeSize_;
    other.typeAlign_ = typeAlign_;
    other.deleter_ = deleter_;
    other.copier_ = copier_;
    other.value_ = copier_(value_);
    return other;
  }

  // any-type constructor
  template <typename T, typename CleanT = std::decay_t<T>,
            std::enable_if_t<!std::is_same_v<CleanT, Data>, bool> = true>
  Data(T &&value,
       std::function<void(void *)> deleter = &Data::CastDelete<CleanT>,
       std::function<void *(void *)> copier = &Data::CastCopy<CleanT>) {
    typeInfo_ = &typeid(value);
    typeSize_ = sizeof(CleanT);
    typeAlign_ = alignof(CleanT);
    deleter_ = deleter;
    copier_ = copier;
    value_ = new CleanT(std::forward<T>(value));
  }

  // type properties
  const std::type_info *typeInfo() const { return typeInfo_; }
  std::size_t typeSize() const { return typeSize_; }
  std::size_t typeAlign() const { return typeAlign_; }

  // data pointer
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

  // any-type constructor
  template <typename T, typename CleanT = std::decay_t<T>,
            std::enable_if_t<!std::is_same_v<CleanT, Var>, bool> = true>
  Var(T &&other) {
    initData(Data{std::forward<T>(other)});
  }

  // value getter
  template <typename T> const T &value() {
    if (&typeid(T) == data_->typeInfo()) {
      return (*data_->ptr<T>());
    }
    throw(std::runtime_error("Var::value unexpected conversion"));
  }

  // operators
  template <typename T> Var operator+(const T &a) {
    return varOperator("+", a);
  }

  template <typename T> Var operator-(const T &a) {
    return varOperator("-", a);
  }
};

std::ostream &operator<<(std::ostream &os, const Var &var);

// Functions
// ----------------------------------------------------------------------------
using Args = std::vector<Data *>;
using TypeInfo = const std::type_info *;
using TypeList = std::initializer_list<TypeInfo>;
typedef Data (*OpFunc)(const Args &args);
typedef Data (*InitFunc)(Data &&data);

void SetContext(const char *ctx);
void SetInitializerFunc(const InitFunc &func, TypeInfo type);
void SetOperatorFunc(OpFunc func, const char *op, const TypeList &types);

template <typename T> void SetInitializerFunc(InitFunc func) {
  SetInitializerFunc(func, &typeid(T));
}

template <typename... T> void SetOperatorFunc(OpFunc func, const char *op) {
  SetOperatorFunc(func, op, TypeList{&typeid(T)...});
}

// Test
// ----------------------------------------------------------------------------
void Test();

} // namespace EtLab
