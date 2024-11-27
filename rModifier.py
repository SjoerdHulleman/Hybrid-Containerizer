import pathlib

import cppModifier
import re

class RFunctionCall:
    def __init__(self, name: str, start_line: int, end_line: int, assigned_var, arguments: list[str]):
        self.name = name
        self.start_line = start_line
        self.end_line = end_line
        self.assigned_var = assigned_var
        self.arguments = arguments

    def __repr__(self):
        return (f"R function call of C++ function on lines {self.start_line} - {self.end_line}:\n"
                f"Arguments:\n"
                f"{self.arguments}\n"
                f"Assigned variable: {self.assigned_var}\n")


def find_function_calls(functions: list[cppModifier.CppFunction], r_file_path: pathlib.Path) -> list[RFunctionCall]:
    function_calls: list[RFunctionCall] = []
    for function in functions:
        temp = find_function_call(function, r_file_path)
        function_calls = function_calls + temp

    return function_calls


def find_function_call(function: cppModifier.CppFunction, r_file_path: pathlib.Path) -> list[RFunctionCall]:
    with open(r_file_path, 'r', encoding='utf-8') as file:
        content = file.read()
        file.seek(0) # Reset pointer to read lines
        lines = file.readlines()

        # Regex to find the START of the function call including the opening parenthesis
        function_start_regex = rf"\b{function.functionName}\s*\("
        function_matches = re.finditer(function_start_regex, content)

        function_calls: list[RFunctionCall] = []

        # Account for multiple calls to the same function
        for function_match in function_matches:

            start_pos = function_match.start()
            # print("Found function:")
            # print (function_match)
            function_call = ''
            start_line = content.count('\n', 0, start_pos) + 1
            end_line = start_line

            # Track parenthesis depth, start at 1 since we start INSIDE the function
            p_depth = 1

            # Create a slice from the file contents, starting from INSIDE the function call
            for index, char in enumerate(content[start_pos + len(function.functionName) + 1:], start_pos + len(function.functionName) + 1):
                if char == '(':
                    p_depth += 1
                elif char == ')':
                    p_depth -= 1

                if p_depth == 0:
                    function_call = content[start_pos:index + 1].strip()
                    # arguments.append(content[start_pos:index + 1].strip())  # Capture arguments part
                    end_line = content.count('\n', 0, index) + 1
                    break

            # print(f"Found function {function.functionName} on lines {start_line}-{end_line} \n Arguments: {function_call}")

            if function_call != '':
                # Extract the arguments from the function call lines
                arguments = extract_arguments(function_call, function.functionName)

                # Extract the assigned variable from the function call line
                assignment_line = re.split(r'<-|=', lines[start_line - 1])
                assigned_variable = re.sub(r'\s*', '', assignment_line[0])

                function_calls.append(RFunctionCall(
                    function.functionName,
                    start_line,
                    end_line,
                    assigned_variable,
                    arguments
                ))
        file.close()
    return function_calls

# Extracts the arguments from a function call
# Inputs:
# - arguments: str -> Ex.: example_function(var1, var2, func1(var3))
# - function_name: str -> Ex.: "example_function"
def extract_arguments(function_call: str, function_name: str) -> list:
    # Remove the function name and any whitespaces inbetween from the start
    function_name_re = re.compile(fr"\s*{function_name}\s*")
    function_call = re.sub(function_name_re, "", function_call)
    # print(function_call)

    # Clean string from whitespaces
    function_call = re.sub(r"\s*", "", function_call)
    # print("Cleaned:")
    # print(function_call)

    # Clean the string of ( and ) on begin and end of string
    if function_call[0] == '(':
        function_call = function_call[1:]
        # Only remove the last ) if we have confirmed there is a ( in front of the string
        if function_call[len(function_call) - 1] == ')':
            function_call = function_call[:-1]

    depth = 0
    start = 0
    result = []
    for index, char in enumerate(function_call):
        if char == '(':
            depth += 1
        if char == ')':
            depth -= 1
        elif char == ',' and depth == 0:
            result.append(function_call[start:index].strip())
            start = index + 1

    result.append(function_call[start:].strip())

    return result


def replace_function_calls(function_calls: list[RFunctionCall], cpp_functions: list[cppModifier.CppFunction], r_file_path: pathlib.Path):
    for function_call in function_calls:
        function = next(function for function in cpp_functions if function.functionName == function_call.name)

        print(f"----- {function.functionName} -----")
        csv_code = "# Convert each param to CSV format\n"
        list_code = (f"# Prepare a list for JSON conversion\n"
                     f"list_{function_call.assigned_var} <- list(\n")

        # For each argument in the function call, create an appropriate data structure
        for index, argument in enumerate(function_call.arguments):
            param = function.parameters[index]

            # Convert the argument into a matrix or vector if applicable
            argument = apply_conversion(param.paramType, argument)

            # Generate the code dependent on current argument
            csv_code += (f"csv_{param.name} <- capture.output(write.csv({argument}, quote=FALSE, row.names=FALSE, col.names=FALSE))\n")
            list_code += f"\t{param.name} = I(paste(csv_{param.name}))"
            if index < len(function_call.arguments) - 1:
                list_code += ",\n"
            else:
                list_code += "\n)"

        # Generate the code dependent on current function call
        json_code = (f"# Convert to JSON format\n"
                     f"json_{function_call.assigned_var} <- toJSON(list_{function_call.assigned_var}, auto_unbox=TRUE)")
        # TODO: Add C++ URL configuration
        url_code = (f"# URL dependent on configuration\n"
                    f"url_{function_call.assigned_var} <- \"http://localhost:8080/{function_call.name}\"\n")
        response_code = (f"# Call the correct API endpoint for this function and process the result\n"
                         f"response_{function_call.assigned_var} <- POST(\n"
                             f"\turl_{function_call.assigned_var},\n"
                             f"\tbody = json_{function_call.assigned_var},\n"
                             f"\tencode = \"raw\",\n"
                             f"\tadd_headers(\"Content-Type\" = \"application/json\")\n"
                             f")\n")
        content_code = f"content_{function_call.assigned_var} <- content(response_{function_call.assigned_var})\n"
        var_code = (f"# Capture the results in the original variable, and convert into correct format\n"
                    f"{function_call.assigned_var} <- {apply_conversion(function.returnType, f"content_{function_call.assigned_var}")}")

        code_block = ("# ----- THIS CODE IS AUTO-GENERATED BY MULTICONTAINERIZER -----\n"
                      + csv_code + "\n" + list_code + "\n" + json_code + "\n" + url_code + "\n" + response_code + "\n" + content_code + "\n" + var_code + "\n" +
                      "# ---- END OF AUTO-GENERATED CODE -----\n")
        print(code_block)


# Input:
# - param: str - Used to determine the type of the argument
# - argument: str - The original argument in the R code
def apply_conversion(paramType: str, argument: str) -> str:
    # Clean the parameter
    cleaned_param = re.sub(r"^\s*const\s*", "", paramType)
    cleaned_param = re.sub(r'&$', '', cleaned_param)

    # Check if it is a Matrix
    re_matrix = re.compile(r'std::vector<std::vector<(.+)>>')
    re_vector = re.compile(r'std::vector<(.+)>')

    matrix_match = re.search(re_matrix, cleaned_param)
    if matrix_match:
        # It is a matrix
        # matrix_elem_type = matrix_match.group(1)

        # Add as.matrix() around argument
        argument = f"as.matrix({argument})"
        return argument

    elif (vector_match := re.search(re_vector, cleaned_param)):
        # It is a vector
        # vector_elem_type = vector_match.group(1)

        # Add as.matrix() around argument
        argument = f"as.vector({argument})"
        return argument

# In other cases, we change nothing


