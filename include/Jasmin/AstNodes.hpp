#pragma once

#include "Common.hpp"

namespace Jasmin
{
namespace AstNodes
{

struct Node 
{
  virtual std::string ToString() const = 0;
  virtual ~Node() = default;
};

template <typename T>
struct Value: public Node
{
  virtual T GetValue() const = 0;
  virtual std::string ToString() const = 0;
};

template <typename T>
struct ImmediateValue : public Value<T>
{
  T Value;

  ImmediateValue(T value) : Value{value} {}

  T GetValue() const override { return Value; }

  std::string ToString() const
  {
    std::stringstream ss;
    ss << "ImmediateValue{";
    ss << this->Value;
    ss << '}';
    return ss.str();
  }
};

enum class ArithmeticOperation
{
  Add,
  Sub,
  Div,
  Mul
};


struct BinaryExpressionCommon
{
  static const std::unordered_map<std::string_view, ArithmeticOperation> CharOpMap;
  static const std::unordered_map<ArithmeticOperation, std::string_view> OpCharMap;
};

template <typename LhsT, typename RhsT, typename CommonType = typename std::common_type<LhsT, RhsT>::type >
struct BinaryExpression : public BinaryExpressionCommon, public Value<CommonType>
{
  ArithmeticOperation Op;
  uPtr< Value<LhsT> > Lhs;
  uPtr< Value<RhsT> > Rhs;

  BinaryExpression(ArithmeticOperation op, Value<LhsT>* lhs, Value<RhsT>* rhs) 
    : Op{op}, Lhs{lhs}, Rhs{rhs} {}

  BinaryExpression(std::string_view op, Value<LhsT>* lhs, Value<RhsT>* rhs) :
    BinaryExpression( BinaryExpressionCommon::CharOpMap.at(op), lhs, rhs) {}

  CommonType GetValue() const override
  {
    switch(this->Op)
    {
      using OP = ArithmeticOperation;
      case OP::Add: return this->Lhs->GetValue() + this->Rhs->GetValue();
      case OP::Sub: return this->Lhs->GetValue() - this->Rhs->GetValue();
      case OP::Div: return this->Lhs->GetValue() / this->Rhs->GetValue();
      case OP::Mul: return this->Lhs->GetValue() * this->Rhs->GetValue();

      default: throw std::runtime_error{"unhandled arithmetic operation type"};
    };
  }

  std::string ToString() const override
  { 
    std::stringstream ss;
    ss << "BinaryExpression{ ";
    ss << Lhs->ToString();
    ss << ' ';
    ss << BinaryExpressionCommon::OpCharMap.at(this->Op);
    ss << ' ';
    ss << Rhs->ToString();
    ss << " }";
    return ss.str();
  }

};

struct Directive: public Node
{
  std::string Name;
  std::vector<std::string> Parameters;

  Directive(std::string name, std::vector<std::string> params);
  std::string ToString() const override;
};

struct Instruction: public Node
{
  std::string Name;

  using ArgT = std::variant<uPtr<Value<std::string>>, uPtr<Value<double>>>;
  std::vector<ArgT> Args;

  Instruction(std::string name);
  std::string ToString() const override;
};

struct Label: public Node
{
  std::string Name;
  std::vector< uPtr<Node> > Body;

  Label(std::string name);
  std::string ToString() const override;
};


} //namespace: AstNodes
} //namespace: Jasmin
