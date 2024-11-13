from plDetector import ProgrammingLanguage
import re
import pathlib

import clang.cindex


class CppParam:
    def __init__(self, name: str, paramType: str):
        self.name = name
        self.paramType = paramType

    def __repr__(self):
        return ("%s : %s" % (self.name, self.paramType))


class CppFunction:
    def __init__(self, functionName: str, returnType, parameters: list[CppParam], startLine: int, endLine: int):
        self.functionName = functionName
        self.returnType = returnType
        self.parameters = parameters
        self.startLine = startLine
        self.endLine = endLine

    def __repr__(self):
        return (("Name: %s \n"
                 "Return type: %s \n"
                 "Params: %s \n"
                 "Lines: %s - %s")
                % (self.functionName, self.returnType, self.parameters, self.startLine, self.endLine))


def find_cpp_file(language: ProgrammingLanguage) -> pathlib.Path:
    print(language.path)
    parent_path = language.path.parent

    cpp_file_name = ''

    with open(language.path, 'r', encoding='utf-8') as file:

        for line in file:
            if 'sourceCpp' in line:
                result = re.sub(r'\s*#.*', '', line)

                cpp_file_name = result.strip().strip("sourceCpp(\"").strip("\")")

    cpp_file_path = pathlib.Path(parent_path) / cpp_file_name

    return cpp_file_path


def strip_comments(cpp_file_path: pathlib.Path) -> list[str]:
    with open(cpp_file_path, 'r', encoding='utf-8') as file:
        # Captures all Rcpp export tags
        rcpp_export_regex = re.compile(r'\s*//\s*\[\[\s*Rcpp::export\s*\]\]\s*\n*\s*')

        # First case for lines with only comments, second line for lines that also contain code,
        # where we do not want to remove the \n.
        comment_regex = re.compile(r'(^\s*//.*\n*)|(\s*//.*)')

        lines = list()

        for line in file:
            # content = file.read()

            # export_tags = re.findall(rcpp_export_regex, content)

            # Check if it is an export line
            res = re.search(rcpp_export_regex, line)
            if res is not None:
                lines.append(res.string)
                continue
            else:
                # Clean comments
                res_line = re.sub(comment_regex, '', line)
                if res_line != '' and res_line != '\n':
                    empty_line_check = re.search(r'^\s*$', res_line)
                    if empty_line_check is None:
                        lines.append(res_line)

        return lines


def write_cpp_file(lines: list[str], cpp_file_path: pathlib):
    with open(cpp_file_path, 'w', encoding='utf-8') as file:
        for line in lines:
            file.write(line)


# Check https://learn.microsoft.com/en-us/cpp/cpp/functions-cpp?view=msvc-170 for function declaration options
def extract_cpp_functions(cpp_file_path: pathlib.Path, llvm_path: pathlib.Path) -> list[str]:
    clang.cindex.Config.set_library_path(llvm_path)

    index = clang.cindex.Index.create()

    translation_unit = index.parse(cpp_file_path, args=['-std=c++17', '-fsyntax-only', '-ferror-limit=0', ])
    functions = []

    extract_functions(translation_unit.cursor, functions, cpp_file_path)

    print(functions)


# def replace_rcpp(cpp_filePath: pathlib.Path):
#


# def extract_functions(node, functions):
#     try:
#         print(f"Processing node kind: {node.kind}")
#         if node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
#             print(node.extent.start.line, " - ", node.extent.end.line)
#             print(node.location.file.name)
#     # Existing function extraction logic...
#     except ValueError as e:
#         print(f"Error encountered: {e}")
#     for child in node.get_children():
#         extract_functions(child, functions)

def extract_functions(node, functions, filepath: pathlib.Path):
    # Match the filepaths and only consider the submitted file, else clang will go over the includes
    if (
            (node.location.file is not None)
            and (pathlib.Path(node.location.file.name).as_posix() == filepath.as_posix())
            and (node.kind == clang.cindex.CursorKind.FUNCTION_DECL)
    ):
        # Get function details
        func_name = node.spelling
        return_type = node.result_type.spelling
        args = [CppParam(arg.spelling, arg.type.spelling) for arg in node.get_arguments()]
        start_line = node.extent.start.line
        end_line = node.extent.end.line

        # Add to functions list in correct format
        functions.append(CppFunction(
            func_name,
            return_type,
            args,
            start_line,
            end_line
        ))
        # Recurse for child nodes
    for child in node.get_children():
        extract_functions(child, functions, filepath)

    # with open(cpp_file_path, 'r', encoding='utf-8') as file:
    #
    #     # for line in file:
