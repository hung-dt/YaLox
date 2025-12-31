import sys
import os


def declareType(file, baseName, className, fieldList):
    fullName = className + baseName
    file.write("/*" + 75 * "-" + "*/\n\n")
    if baseName == "Expr":
        file.write(f"/** {className} expression.\n */\n")
    else:
        file.write(f"/** {className} statement.\n */\n")
    file.write(f"class {fullName} : public {baseName}\n")
    file.write("{\npublic:\n")

    fieldNames = [e[0] for e in fieldList]
    ctorTypes = [e[1] for e in fieldList]
    memberTypes = []
    for e in fieldList:
        if len(e) < 3:
            memberTypes.append(e[1])
        else:
            memberTypes.append(e[2])

    # Write constructor line
    ctor = f"  {fullName}("
    for idx in range(len(fieldList)):
        ctor += f"{ctorTypes[idx]} {fieldNames[idx]}, "
    ctor = ctor[:-2] + ");\n\n"
    file.write(ctor)

    # Write visitor's overriden methods
    if baseName == "Expr":
        file.write("  std::string toString(AstPrinter&) override;\n\n")
        file.write("  LoxObject evaluate(Interpreter&) override;\n\n")
    else:
        file.write("  void execute(Interpreter&) override;\n\n")

    # Write member lines
    lines = ""
    for idx in range(len(fieldList)):
        lines += f"  {memberTypes[idx]} {fieldNames[idx]};\n"
    file.write(lines)

    file.write("};\n\n")


def defineType(file, baseName, className, fieldList):
    fullName = className + baseName
    fieldNames = [e[0] for e in fieldList]
    ctorTypes = [e[1] for e in fieldList]

    file.write("/*" + 75 * "-" + "*/\n\n")
    ctor = f"{fullName}::{fullName}("
    for idx in range(len(fieldList)):
        ctor += f"{ctorTypes[idx]} {fieldNames[idx]}, "
    ctor = ctor[:-2] + ")\n  : "
    for idx in range(len(fieldList)):
        if "&" not in ctorTypes[idx]:
            ctor += f"{fieldNames[idx]}(std::move({fieldNames[idx]})), "
        else:
            ctor += f"{fieldNames[idx]}({fieldNames[idx]}), "
    ctor = ctor[:-2] + "\n{\n}\n\n"
    file.write(ctor)

    if baseName == "Expr":
        file.write("/*" + 75 * "-" + "*/\n\n")
        file.write(f"std::string {fullName}::toString(AstPrinter& printer)\n")
        file.write(f"{{\n  return printer.visit{fullName}(*this);\n}}\n\n")

        file.write("/*" + 75 * "-" + "*/\n\n")
        file.write(f"LoxObject {fullName}::evaluate(Interpreter& interpreter)\n")
        file.write(f"{{\n  return interpreter.visit{fullName}(*this);\n}}\n\n")
    else:
        file.write("/*" + 75 * "-" + "*/\n\n")
        file.write(f"void {fullName}::execute(Interpreter& interpreter)\n")
        file.write(f"{{\n  interpreter.visit{fullName}(*this);\n}}\n\n")


def defineVisitor(file, baseName, types):
    file.write("/*" + 75 * "-" + "*/\n\n")
    file.write("template <typename T>\n")
    file.write(f"class {baseName}Visitor\n")
    file.write("{\npublic:\n")
    file.write(f"  virtual ~{baseName}Visitor() = default;\n\n")
    for t in types:
        fullName = t + baseName
        file.write(f"  virtual T visit{fullName}({fullName}&) = 0;\n")
    file.write("};\n\n")
    if baseName == "Expr":
        file.write("/*" + 75 * "-" + "*/\n\n")
        file.write(f"// Forward declare {baseName}Visitor implementations\n")
        file.write("class AstPrinter;\n")
        file.write("class Interpreter;\n\n")


def defineAst(outputDir, baseName, types):
    # generate .hpp header file
    with open(
        os.path.join(outputDir, baseName.lower() + ".hpp"), "w", encoding="utf-8"
    ) as f:
        f.write("#pragma once\n\n")
        if baseName == "Stmt":
            f.write('#include "expr.hpp"\n\n')
        else:
            f.write('#include "token.hpp"\n\n')
        f.write("#include <vector>\n")
        f.write("#include <memory>\n\n")
        f.write("namespace lox {\n\n")
        f.write("/*" + 75 * "-" + "*/\n\n")
        f.write(f"// Forward declare all {baseName} types\n")
        f.write(f"class {baseName};\n")

        classNames = [e["name"] for e in types]
        classFields = [e["params"] for e in types]

        for name in classNames:
            f.write(f"class {name}{baseName};\n")
        f.write("\n")
        f.write(f"using {baseName}Ptr = std::unique_ptr<{baseName}>;\n\n")

        defineVisitor(f, baseName, classNames)

        f.write("/*" + 75 * "-" + "*/\n\n")
        if baseName == "Expr":
            f.write("/** Base class for all expression types.\n */\n")
        else:
            f.write("/** Base class for all statement types.\n */\n")
        f.write(f"class {baseName}\n")
        f.write("{\n")
        f.write("public:\n")
        f.write(f"  virtual ~{baseName}() = default;\n\n")
        if baseName == "Expr":
            f.write(f"  // accept function for {baseName}Visitor\n")
            f.write("  virtual std::string toString(AstPrinter&) = 0;\n\n")
            f.write(f"  // accept function for {baseName}Visitor\n")
            f.write("  virtual LoxObject evaluate(Interpreter&) = 0;\n")
        else:
            f.write(f"  // accept function for {baseName}Visitor\n")
            f.write("  virtual void execute(Interpreter&) = 0;\n")
        f.write("};\n\n")

        for idx, name in enumerate(classNames):
            declareType(f, baseName, name, classFields[idx])
        f.write("}  // namespace lox\n")

    # generate .cpp implementation file
    with open(
        os.path.join(outputDir, baseName.lower() + ".cpp"), "w", encoding="utf-8"
    ) as f:
        f.write(f'#include "{baseName.lower()}.hpp"\n\n')
        if baseName == "Expr":
            f.write('#include "astprinter.hpp"\n')
        f.write('#include "interpreter.hpp"\n\n')
        f.write("namespace lox {\n\n")
        for idx, name in enumerate(classNames):
            defineType(f, baseName, name, classFields[idx])
        f.write("}  // namespace lox\n")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: generate_ast.py <output_dir>")
        sys.exit(1)

    outputDir = sys.argv[1]
    exprTypes = [
        {"name": "Assign", "params": [["name", "Token"], ["value", "ExprPtr"]]},
        {
            "name": "Binary",
            "params": [
                ["left", "ExprPtr"],
                ["op", "Token"],
                ["right", "ExprPtr"],
            ],
        },
        {
            "name": "Call",
            "params": [
                ["callee", "ExprPtr"],
                ["closingParen", "Token"],
                ["arguments", "std::vector<ExprPtr>"],
            ],
        },
        {
            "name": "Grouping",
            "params": [
                ["expression", "ExprPtr"],
            ],
        },
        {
            "name": "Literal",
            "params": [
                ["value", "LoxObject"],
            ],
        },
        {
            "name": "Logical",
            "params": [
                ["left", "ExprPtr"],
                ["op", "Token"],
                ["right", "ExprPtr"],
            ],
        },
        {
            "name": "Unary",
            "params": [
                ["op", "Token"],
                ["right", "ExprPtr"],
            ],
        },
        {"name": "Variable", "params": [["name", "Token"]]},
    ]
    defineAst(outputDir, "Expr", exprTypes)

    stmtTypes = [
        {"name": "Block", "params": [["statements", "std::vector<StmtPtr>"]]},
        {"name": "Expr", "params": [["expression", "ExprPtr"]]},
        {
            "name": "Function",
            "params": [
                ["name", "Token"],
                ["params", "std::vector<Token>"],
                ["body", "std::vector<StmtPtr>"],
            ],
        },
        {
            "name": "If",
            "params": [
                ["condition", "ExprPtr"],
                ["thenBranch", "StmtPtr"],
                ["elseBranch", "StmtPtr"],
            ],
        },
        {"name": "Print", "params": [["expression", "ExprPtr"]]},
        {"name": "Var", "params": [["name", "Token"], ["initializer", "ExprPtr"]]},
        {"name": "While", "params": [["condition", "ExprPtr"], ["body", "StmtPtr"]]},
        {
            "name": "For",
            "params": [
                ["initializer", "StmtPtr"],
                ["condition", "ExprPtr"],
                ["increment", "ExprPtr"],
                ["body", "StmtPtr"],
            ],
        },
    ]
    defineAst(outputDir, "Stmt", stmtTypes)
