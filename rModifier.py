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
        # For each argument in the function call, create an appropriate data structure
        for index, argument in enumerate(function_call.arguments):
            param = function.parameters[index]
            cleaned_argument = re.sub(r"^\s*const\s*", "", param.paramType)
            cleaned_argument = re.sub

            print(cleaned_argument)

            # print(function.parameters[index])
            # Write logic to replace the call depending on the type

            # Check if it is a Matrix


            # Check if it is a Vector





