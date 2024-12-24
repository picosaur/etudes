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
  OwningMaker<Data> datas_;
  FunctionList<InitFunc> inits_;
  FunctionList<OperFunc> opers_;

public:
  Data *makeData() { return datas_.make(); }
  Data *makeData(Data &&data) { return datas_.make(std::forward<Data>(data)); }
  void acquireData(Data *data) { datas_.acquire(data); }
  void releaseData(Data *data) { datas_.release(data); }

  void setInitFunc(InitFunc func, TypeInfo type) {
    inits_.insert(hash_combine(0, type), func);
  }

  InitFunc getInitFunc(TypeInfo type) {
    auto it = inits_.find(hash_combine(0, type));
    if (inits_.isFound(it)) {
      return (it->second);
    }
    return BypassInit;
  }

  static Data BypassInit(Data &&data) { return std::forward<Data>(data); }

  void setOpFunc(OperFunc func, const char *op, const TypeList &types) {
    opers_.insert(hash_combine_list(hash_combine(0, op), types), func);
  }

  OperFunc getOpFunc(const char *op, const TypeList &types) {
    auto it = opers_.find(hash_combine_list(hash_combine(0, op), types));
    if (opers_.isFound(it)) {
      return (it->second);
    }
    return DummyBinOp;
  }

  static Data DummyBinOp(const ArgsList &args) { return {}; }
};

using EngineContext = SelectableContext<Engine>;

// Var
// ----------------------------------------------------------------------------
Var::Var() { initEmptyData(); }

Var::~Var() {
  auto ctx = EngineContext::GetContext();
  ctx->releaseData(data_);
}

Var::Var(const Var &arg) {
  auto ctx = EngineContext::GetContext();
  ctx->acquireData(arg.data_);
  data_ = arg.data_;
}

void Var::operator=(const Var &arg) {
  auto ctx = EngineContext::GetContext();
  ctx->acquireData(arg.data_);
  data_ = arg.data_;
}

void Var::initEmptyData() {
  auto ctx = EngineContext::GetContext();
  data_ = ctx->makeData();
}

void Var::initData(Data &&data) {
  auto ctx = EngineContext::GetContext();
  auto initFunc = ctx->getInitFunc(data.typeInfo());
  data_ = ctx->makeData(initFunc(std::forward<Data>(data)));
}

Var Var::unaryOperator(const char *op) const {
  auto ctx = EngineContext::GetContext();
  auto func = ctx->getOpFunc(op, {data_->typeInfo()});
  return func(ArgsList{data_});
}

Var Var::binaryOperator(const char *op, const Var &arg) const {
  auto ctx = EngineContext::GetContext();
  auto func = ctx->getOpFunc(op, {data_->typeInfo(), arg.data_->typeInfo()});
  return func(ArgsList{data_, arg.data_});
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

void SetInitializerFunc(const InitFunc &func, TypeInfo type) {
  auto *ctx = EngineContext::GetContext();
  ctx->setInitFunc(func, type);
}

void SetOperatorFunc(OperFunc func, const char *op, const TypeList &types) {
  auto *ctx = EngineContext::GetContext();
  ctx->setOpFunc(func, op, types);
}

// Test
// ----------------------------------------------------------------------------
class CustomData {};

void Test() {
  SetInitializerFunc<int>([](Data &&data) {
    double v = (*data.ptr<int>());
    return Data(v);
  });

  SetOperatorFunc<double>(
      [](const ArgsList &args) { return Data{-(*args.at(0)->ptr<double>())}; },
      "-");

  SetOperatorFunc<double, double>(
      [](const ArgsList &args) {
        return Data{*args.at(0)->ptr<double>() + *args.at(1)->ptr<double>()};
      },
      "+");

  double aaa;
  Data asd(aaa);

  Var a = 10;
  Var b = -a;
  Var c = 11 + a + b;
  std::cout << c.value<double>() << std::endl;
}

} // namespace EtLab
