#include "AST.hpp"
#include "Lexer.hpp"
#include <llvm-18/llvm/IR/BasicBlock.h>
#include <llvm-18/llvm/IR/Constants.h>
#include <llvm-18/llvm/IR/DerivedTypes.h>
#include <llvm-18/llvm/IR/Function.h>
#include <llvm-18/llvm/IR/Instructions.h>
#include <llvm-18/llvm/IR/Type.h>
#include <llvm-18/llvm/IR/Value.h>
#include <llvm-18/llvm/Support/raw_ostream.h>
#include <llvm-c-18/llvm-c/Types.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <variant>

struct Symbol {
  llvm::Value *address;
  llvm::Type *type;
};

class CodeGen {
public:
  CodeGen() : m_Builder(m_Context) {
    m_Module = std::make_unique<llvm::Module>("kol_lang", m_Context);
  }

  void generate(const Program &program) { generateMain(program.s_Main); }

  void dump() { m_Module->print(llvm::outs(), nullptr); }

private:
  llvm::LLVMContext m_Context;
  llvm::IRBuilder<> m_Builder;
  std::unique_ptr<llvm::Module> m_Module;

  std::unordered_map<std::string, Symbol> m_NamedValues;

  llvm::Function *m_MainFunction = nullptr;

  void generateMain(const MainFunction &main) {

    llvm::FunctionType *mainType =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(m_Context), false);

    m_MainFunction = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", m_Module.get());

    llvm::BasicBlock *entry =
        llvm::BasicBlock::Create(m_Context, "entry", m_MainFunction);

    m_Builder.SetInsertPoint(entry);

    for (const auto &statement : main.s_Statements) {
      generateStatement(statement);

      if (m_Builder.GetInsertBlock()->getTerminator()) {
        break;
      }
    }
  }

  llvm::Value *generateExpression(const Expression &expr) {
    return std::visit(
        [&](const auto &exp) -> llvm::Value * {
          using T = std::decay_t<decltype(exp)>;

          if constexpr (std::is_same_v<T, IntegerLiteral>) {
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_Context),
                                          exp.s_Value);
          } else if constexpr (std::is_same_v<T, BooleanLiteral>) {
            return llvm::ConstantInt::get(llvm::Type::getInt1Ty(m_Context),
                                          exp.s_Value);
          } else if constexpr (std::is_same_v<T, FloatLiteral>) {
            return llvm::ConstantFP::get(llvm::Type::getDoubleTy(m_Context),
                                         exp.s_Value);
          } else if constexpr (std::is_same_v<T, StringLiteral>) {
            return m_Builder.CreateGlobalStringPtr(exp.s_Value);
          } else if constexpr (std::is_same_v<T, VariableExpression>) {
            auto it = m_NamedValues.find(exp.s_Name);

            if (it == m_NamedValues.end()) {
              throw std::runtime_error("unknown Var" + exp.s_Name);
            }

            Symbol &symbol = it->second;

            return m_Builder.CreateLoad(symbol.type, symbol.address,
                                        exp.s_Name);
          }
        },
        expr);
  }

  void generateStatement(const Statement &statement) {
    std::visit(
        [&](const auto &stmnt) {
          using T = std::decay_t<decltype(stmnt)>;

          if constexpr (std::is_same_v<T, ReturnStatement>) {
            llvm::Value *value = generateExpression(stmnt.s_Expression);

            m_Builder.CreateRet(value);
          } else if constexpr (std::is_same_v<T, VariableDeclaration>) {
            generateVariableDeclaration(stmnt);
          }
        },
        statement);
  }

  llvm::Type *llvmType(TokenType type) {
    switch (type) {
    case TokenType::INT_T:
      return llvm::Type::getInt32Ty(m_Context);
    case TokenType::BOOL:
      return llvm::Type::getInt1Ty(m_Context);
    case TokenType::FLOAT_T:
      return llvm::Type::getDoubleTy(m_Context);
    case TokenType::STR_T:
      return llvm::PointerType::getUnqual(m_Context);
    default:
      throw std::runtime_error("Type ERROR"

      );
    }
  }

  void generateVariableDeclaration(const VariableDeclaration &decl) {
    llvm::Type *type = llvmType(decl.s_Type);

    llvm::AllocaInst *allocation =
        m_Builder.CreateAlloca(type, nullptr, decl.s_Name);

    llvm::Value *value = generateExpression(decl.s_Initializer);

    m_Builder.CreateStore(value, allocation);

    m_NamedValues[decl.s_Name] = {allocation, type};
  }
};
