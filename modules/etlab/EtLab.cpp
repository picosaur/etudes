#include "EtLab.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace EtLab {

// LinearMap
// ----------------------------------------------------------------------------
template <typename Tk, typename Tv> class LinearMap {
  using TItem = std::pair<Tk, std::unique_ptr<Tv>>;
  using TList = std::vector<TItem>;
  TList list_;

public:
  typename TList::iterator find(const Tk &key) {
    return std::find_if(
        list_.begin(), list_.end(),
        [key = key](const TItem &item) { return item.first == key; });
  }

  bool isFound(const typename TList::iterator &it) const {
    return it != list_.end();
  }

  bool contains(const Tk &key) const { return isFound(find(key)); }

  Tv *at(const Tk &key) {
    auto it = find(key);
    if (!isFound(it)) {
      list_.push_back({key, std::make_unique<Tv>()});
      it = std::prev(list_.end());
    }
    return it->second.get();
  }
};

// SelectableContext
// ----------------------------------------------------------------------------
template <typename T> class SelectableContext {
  LinearMap<std::size_t, T> map_;
  std::size_t curr_{};

  SelectableContext() {}

  static SelectableContext &Instance() {
    static SelectableContext instance;
    return instance;
  }

  static T *Get(std::size_t id) { return Instance().map_.at(id); }

public:
  SelectableContext(SelectableContext const &) = delete;
  void operator=(SelectableContext const &) = delete;

  static void SetContext(std::size_t id) { Instance().curr_ = id; }

  static T *GetContext() { return Get(Instance().curr_); }
};

// OwningMaker
// ----------------------------------------------------------------------------
template <typename Tv> class OwningMaker {
  class Item {
  public:
    std::size_t counter{};
    std::unique_ptr<Tv> value;

    bool operator==(const Item &otherPoint) const {
      return value.get() == value.get();
    }
  };

  using TList = std::unordered_map<Tv *, Item>;
  TList list_;

  typename TList::iterator findExisting(Tv *ptr) {
    auto it = list_.find(ptr);
    if (it == list_.end()) {
      throw std::out_of_range("OwningMaker::findExisting out of range");
    }
    return it;
  }

public:
  Tv *make() {
    auto item = Item{.counter = 1, .value = std::make_unique<Tv>()};
    auto ptr = item.value.get();
    list_.insert({ptr, std::move(item)});
    return ptr;
  }

  Tv *make(Tv &&other) {
    auto item =
        Item{.counter = 1, .value = std::make_unique<Tv>(std::move(other))};
    auto ptr = item.value.get();
    list_.insert({ptr, std::move(item)});
    return ptr;
  }

  void acquire(Tv *ptr) {
    auto it = findExisting(ptr);
    it->second.counter++;
  }

  void release(Tv *ptr) {
    auto it = findExisting(ptr);
    it->second.counter--;
    if (it->second.counter == 0) {
      list_.erase(it);
    }
  }
};

// hash_combine
// ----------------------------------------------------------------------------
inline std::size_t hash_combine(std::size_t seed) { return seed; }

template <typename T, typename... Rest>
inline std::size_t hash_combine(std::size_t seed, const T &v, Rest... rest) {
  seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return hash_combine(seed, rest...);
}

template <typename T>
inline std::size_t hash_combine_list(std::size_t seed,
                                     const std::initializer_list<T> &v) {
  for (const auto &item : v) {
    seed = hash_combine(seed, item);
  }
  return seed;
}

// FunctionList
// ----------------------------------------------------------------------------
template <typename Tfunc> class FunctionList {
  using Tcontainer = std::unordered_map<std::size_t, Tfunc>;
  Tcontainer map_;

public:
  void insert(std::size_t key, const Tfunc &func) { map_.insert({key, func}); }

  typename Tcontainer::iterator find(std::size_t key) { return map_.find(key); }

  bool isFound(const typename Tcontainer::iterator &it) const {
    return it != map_.end();
  }
};

// Engine
// ----------------------------------------------------------------------------
class Engine {
  OwningMaker<Data> vars_;
  FunctionList<OpFunc> ops_;

public:
  Data *makeVar() { return vars_.make(); }
  Data *makeVar(Data &&data) { return vars_.make(std::move(data)); }
  void acquireVar(Data *data) { vars_.acquire(data); }
  void releaseVar(Data *data) { vars_.release(data); }

  void setOpFunc(OpFunc func, const char *op, const TypeList &types) {
    ops_.insert(hash_combine_list(hash_combine(0, op), types), func);
  }

  OpFunc getOpFunc(const char *op, const TypeList &types) {
    auto it = ops_.find(hash_combine_list(hash_combine(0, op), types));
    if (ops_.isFound(it)) {
      return (it->second);
    }
    return DummyBinOp;
  }

  static Data DummyBinOp(const Args &args) { return {}; }
};

using EngineContext = SelectableContext<Engine>;

// Var
// ----------------------------------------------------------------------------
Var::Var() { initEmptyData(); }

Var::~Var() {
  auto ctx = EngineContext::GetContext();
  ctx->releaseVar(data_);
}

Var::Var(const Var &arg) {
  auto ctx = EngineContext::GetContext();
  ctx->acquireVar(arg.data_);
  data_ = arg.data_;
}

void Var::operator=(const Var &arg) {
  auto ctx = EngineContext::GetContext();
  ctx->acquireVar(arg.data_);
  data_ = arg.data_;
}

void Var::initEmptyData() {
  auto ctx = EngineContext::GetContext();
  data_ = ctx->makeVar();
}

void Var::initData(Data &&data) {
  auto ctx = EngineContext::GetContext();
  data_ = ctx->makeVar(std::forward<Data>(data));
}

Var Var::varOperator(const char *op, const Var &a) {
  auto ctx = EngineContext::GetContext();
  auto func = ctx->getOpFunc("+", {data_->typeInfo(), a.data_->typeInfo()});
  return func({data_, a.data_});
}

std::ostream &operator<<(std::ostream &os, const Var &var) {
  os << std::to_string(reinterpret_cast<uintptr_t>(var.data_));
  return os;
}

// Functions
// ----------------------------------------------------------------------------
void SetContext(const char *ctx) {
  EngineContext::SetContext(std::hash<const char *>()(ctx));
}

void SetOperatorFunc(const OpFunc &func, const char *op,
                     const TypeList &typeList) {
  auto *ctx = EngineContext::GetContext();
}

// Test
// ----------------------------------------------------------------------------
class CusomData {};

void Test() {
  /*
  SetOperatorFunc(
      [](const Args &args) {
        return Data{*args.at(0)->ptr<double>() + *args.at(1)->ptr<double>()};
      },
      "+", {&typeid(double), &typeid(double)});
*/
  double aaa;
  Data asd(aaa);

  Var a = 10;
  Var b = a;
  Var c = a + 10 + b;
  Var d = CusomData();
  std::cout << c << std::endl;
}

} // namespace EtLab
