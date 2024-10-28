
from plDetector import ProgrammingLanguage
import re
import pathlib

import clang.cindex

class CppFunction:
    def __init__(self, functionName: str, parameters, output, body):
        self.functionName = functionName
        self.parameters = parameters
        self.output = output
        self.body = body

    def __repr__(self):
        return self.functionName

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
def extract_cpp_functions(cpp_file_path: pathlib.Path) -> list[str]:
    # Prerequisite is to have CastXML installed on machine
    # generator_path = pygccxml.utils.find_xml_generator("castxml")
    # xml_generator_config = pygccxml.parser.xml_generator_configuration_t(
    #     xml_generator_path=generator_path,
    #     xml_generator="castxml"
    # )
    #
    # declarations = pygccxml.parse([cpp_file_path], xml_generator_config)
    #
    # print(pygccxml.declarations.get_free_functions(declarations))


    # With clang
    clang.cindex.Config.set_library_path(r"C:\Program Files\LLVM\bin")

    index = clang.cindex.Index.create()

    translation_unit = index.parse(cpp_file_path)
    functions = []

    extract_functions(translation_unit.cursor, functions)

    print(functions)

def extract_functions(node, functions):
    if node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
        # Get function details
        func_name = node.spelling
        return_type = node.result_type.spelling
        args = [(arg.spelling, arg.type.spelling) for arg in node.get_arguments()]
        functions.append((func_name, return_type, args))
        # Recurse for child nodes
    for child in node.get_children():
        extract_functions(child, functions)

    # with open(cpp_file_path, 'r', encoding='utf-8') as file:
    #
    #     # for line in file:

