import sys
import os


def declareType(file, baseName, className, fieldList):
    fullName = className + baseName
    file.write("/*" + 75 * "-" + "*/\n\n")
    file.write(f"/** {className} expression.\n */\n")
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

    # Write visitor's overriden method
    file.write("  std::string toString(AstPrinter&) override;\n\n")

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
            ctor += f"{fieldNames[idx]}( std::move({fieldNames[idx]}) ), "
        else:
            ctor += f"{fieldNames[idx]}( {fieldNames[idx]} ), "
    ctor = ctor[:-2] + "\n{\n}\n\n"
    file.write(ctor)

    file.write("/*" + 75 * "-" + "*/\n\n")
    file.write(f"std::string {fullName}::toString(AstPrinter& printer)\n")
    file.write(f"{{\n  return printer.visit{fullName}(*this);\n}}\n\n")


def defineVisitor(file, baseName, types):
    file.write("/*" + 75 * "-" + "*/\n\n")
    file.write("template <typename T>\n")
    file.write(f"class {baseName}Visitor\n")
    file.write("{\npublic:\n")
    for t in types:
        fullName = t + baseName
        file.write(f"  virtual T visit{fullName}({fullName}&) = 0;\n")
    file.write("};\n\n")
    file.write("class AstPrinter;\n\n")


def defineAst(outputDir, baseName, types):
    # generate .hpp header file
    with open(
        os.path.join(outputDir, baseName.lower() + ".hpp"), "w", encoding="utf-8"
    ) as f:
        f.write("#pragma once\n\n")
        f.write('#include "token.hpp"\n\n')
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
        f.write("/** Base class for all expression types.\n */\n")
        f.write(f"class {baseName}\n")
        f.write("{\n")
        f.write("public:\n")
        f.write("  virtual std::string toString(AstPrinter&) = 0;\n")
        f.write("};\n\n")

        for idx, name in enumerate(classNames):
            declareType(f, baseName, name, classFields[idx])
        f.write("} // namespace lox\n")

    # generate .cpp implementation file
    with open(
        os.path.join(outputDir, baseName.lower() + ".cpp"), "w", encoding="utf-8"
    ) as f:
        f.write(f'#include "{baseName.lower()}.hpp"\n\n')
        f.write('#include "astprinter.hpp"\n\n')
        f.write("namespace lox {\n\n")
        for idx, name in enumerate(classNames):
            defineType(f, baseName, name, classFields[idx])
        f.write("} // namespace lox\n")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: generate_ast.py <output_dir>")
        sys.exit(1)

    outputDir = sys.argv[1]
    types = [
        {
            "name": "Binary",
            "params": [
                ["left", "ExprPtr"],
                ["op", "const Token&", "Token"],
                ["right", "ExprPtr"],
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
                ["value", "const LoxObject&", "LoxObject"],
            ],
        },
        {
            "name": "Unary",
            "params": [
                ["op", "const Token&", "Token"],
                ["right", "ExprPtr"],
            ],
        },
    ]
    defineAst(outputDir, "Expr", types)
