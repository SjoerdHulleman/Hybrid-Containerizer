from plDetector import ProgrammingLanguage
import re
import pathlib
import shutil

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
def extract_cpp_functions(cpp_file_path: pathlib.Path, llvm_path: pathlib.Path) -> list[CppFunction]:
    clang.cindex.Config.set_library_path(llvm_path)

    index = clang.cindex.Index.create()

    translation_unit = index.parse(cpp_file_path, args=['-std=c++17', '-fsyntax-only', '-ferror-limit=0', ])
    functions = []

    extract_functions(translation_unit.cursor, functions, cpp_file_path)

    return functions


def replace_rcpp(cpp_filePath: pathlib.Path):
    print("Feature to be added in later stage")

def extract_functions(node, functions, filepath: pathlib.Path):
    # Match the filepaths and only consider the submitted file (second statement), else clang will go over the includes
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


def create_api(source_cpp_path: pathlib.Path, base_cpp_path: pathlib.Path, functions: list[CppFunction], use_json: bool = False):
    # Copy the base to the output directory so we can start modifying
    output_path = pathlib.Path('./output/modified.cpp')

    if use_json:
        output_path = pathlib.Path('./CppProjectOutputJSON/modified.cpp')
    else:
        output_path = pathlib.Path('./CppProjectOutputCSV/modified.cpp')

    shutil.copyfile(base_cpp_path, output_path)
    with open(source_cpp_path, 'r') as source_file:
        source_lines = source_file.readlines()
        template_lines = ""

        # Read the output file lines to determine function placement
        with open(output_path, 'r') as output_file:
            # Read lines and add functions
            template_lines = output_file.readlines()
            add_functions_to_cpp(functions, template_lines, source_lines, output_path)

            # Reset pointer and read lines again and add API endpoints
            output_file.seek(0)
            template_lines = output_file.readlines()
            add_api_endpoints_to_cpp(functions, template_lines, output_path, use_json)

    source_file.close()


# Adds al original functions to the template file
def add_functions_to_cpp(functions: list[CppFunction], template_lines: list[str], source_lines: list[str], target_file: pathlib.Path):
    function_lines = ""

    # Create a string of all needed C++ functions
    for function in functions:
        # Add lines of the function
        for i in range(function.startLine - 1, function.endLine):
            function_lines += source_lines[i]

        # Add a new line between functions
        function_lines += "\n"

    # Find marker for correct function insertion and insert
    marker = '// # MARKER: TARGET CPP FUNCTIONS #\n'
    marker_index = template_lines.index(marker) + 1
    template_lines.insert(marker_index, function_lines)

    # Write the functions in the output file
    with open(target_file, 'w') as output_file:
        for line in template_lines:
            output_file.write(line)


def add_api_endpoints_to_cpp(functions: list[CppFunction], template_lines: list[str], target_file: pathlib.Path, use_json: bool = False):
    api_lines = ""
    for function in functions:
        api_lines += (f'\tCROW_ROUTE(app, "/{function.functionName}")\n'
                      '\t\t.methods("POST"_method)\n'
                      '\t\t([](const crow::request& req) {\n'
                      '\n'
                      f'\t\tstd::cout << "Received request for {function.functionName}" << std::endl;\n'
                      '\n'
                      '\t\tcrow::json::rvalue jsonBody = crow::json::load(req.body);\n'
                      '\n'
                      '\t\tif (!jsonBody) {\n'
                      '\t\t\tthrow std::invalid_argument("Invalid JSON format");\n'
                      '\t\t}\n'
                      '\n'
                      '\t\t// ----- Auto generate inputs -----\n'
                      '\t\tstd::list<std::string> inputs = {'
                      )
        for index, param in enumerate(function.parameters):
            api_lines += f'"{param.name}"'
            if index < len(function.parameters) - 1:
                api_lines += ","

        api_lines += ('};\n'
                      '\t\t// ----- End of auto generate inputs -----\n'
                      '\n'
                      '\t\tstd::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);\n'
                      '\n'
                      '\t\t// ----- Auto generate assigned converted results and function call -----\n')
        param_names = ""
        for index, param in enumerate(function.parameters):
            api_lines += f"\t\t{param.paramType} {param.name} = std::any_cast<{param.paramType}>(converted_inputs[{index}]);\n"
            param_names += param.name
            if index < len(function.parameters) - 1:
                param_names += ","

        api_lines += (f'\t\t{function.returnType} result = {function.functionName}({param_names});\n'
                      f'\t\t// ----- End of auto generate assigned converted results and function call -----\n'
                      '\n')

        if use_json:
            api_lines += (
                      f'\t\tjson json_result = result;\n'
                      f'\t\tcrow::response response(json_result.dump());\n'
                      f'\t\tresponse.add_header("Content-Type", "application/json");\n'
            )
        else:
            api_lines += (
                      f'\t\tstd::string csv_result = toCSV(result);\n'
                      '\n'
                      f'\t\tcrow::response response(csv_result);\n'
                      f'\t\tresponse.add_header("Content-Type", "text/csv");\n'
                      '\n'
            )
        api_lines += (
                      f'\t\tstd::cout << "Sending response for {function.functionName}" << std::endl;\n'
                      '\n'
                      '\t\treturn response;\n'
                      '\t});\n\n'
        )

    # Find marker for correct function insertion and insert
    marker = '    // # MARKER: API ENDPOINTS #\n'
    marker_index = template_lines.index(marker) + 1
    template_lines.insert(marker_index, api_lines)

    # Write the API endpoints to the target file
    with open(target_file, 'w') as output_file:
        for line in template_lines:
            output_file.write(line)









