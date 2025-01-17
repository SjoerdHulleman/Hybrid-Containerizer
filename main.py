import os
import pathlib

import cppModifier
import packageDetector
import plDetector
import rContainerizer
import rModifier
import time
# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

from plDetector import analyze_notebook, detect_extension
from packageDetector import find_R_packages

import subprocess
import yaml
import requests
import nbformat
import json


def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


def convert_rmd():
    command = "knitr::purl('data/05_IntegratedPPems_small.Rmd', output='data/05_IntegratedPPems_small.R')"
    result = subprocess.run(["Rscript", "-e", command], capture_output=True, text=True)
    print(result)


# def program_R(pl, packages):
#     if 'Rcpp' in packages:


# def modify_r(r_file, cpp_file):
#     r_file_path = pathlib.Path(r_file)
#     cpp_file_path = pathlib.Path(cpp_file)
#
#

def program_temp(file_path_input):
    path = pathlib.Path(file_path_input)
    llvm_path = ''

    with open("config.yaml", "r") as yaml_file:
        config = yaml.safe_load(yaml_file)
        llvm_path = config["environment"]["llvm_path"]

    if llvm_path is None or llvm_path == '':
        raise Exception("LLVM path is not specified in config.yaml")

    functions = cppModifier.extract_cpp_functions(path, llvm_path)

    r_file_path = pathlib.Path("data/05_IntegratedPPems_small.R")

    print(functions)
    function_calls = []
    for function in functions:
        function_call = rModifier.find_function_call(function, r_file_path)
        function_calls.append(function_call)

    for function_call in function_calls:
        print(function_call)
        print("")
        print("--------------------------")
        print("")


def path_test(file_path_input):
    path = pathlib.Path(file_path_input)

    for part in path.parts:
        print(part)

    print(path.parts[len(path.parts) - 2])


def program():
    start_time = time.time()

    # Initialize config variables
    path = ''
    llvm_path = ''
    use_json = False
    folder = ''
    containerize_R = False

    with open("config.yaml", "r") as yaml_file:
        config = yaml.safe_load(yaml_file)
        llvm_path = config["environment"]["llvm_path"]
        use_json = config["settings"]["use_json"]
        containerize_R = config["settings"]["containerize_R"]

        file_path_input = config["input"]["input_script"]

    if file_path_input is None or file_path_input == '':
        raise Exception("Input script is not specified in config.yaml")
    if llvm_path is None or llvm_path == '':
        raise Exception("LLVM path is not specified in config.yaml")

    path = pathlib.Path(file_path_input)

    if len(path.parts) > 2:
        # Assuming we always use "input/folderName/rFile.R" structure
        folder = path.parts[len(path.parts) - 2]

    # Check which programming language we have
    pl = plDetector.detect_langauge(path)

    # Feed to the package detector to find supported packages
    used_supported_packages = packageDetector.find_used_supported_packages(pl)

    if pl.language == 'R':
        # program_R(pl, used_supported_packages)
        cpp_file_path = cppModifier.find_cpp_file(pl)
        print(f"Found C++ file path: {cpp_file_path.as_posix()}")

        stripped_cpp = cppModifier.strip_comments(cpp_file_path)
        print("Removed comments from C++ file")

        modified_cpp_name = cpp_file_path.with_suffix('')
        modified_cpp_name = modified_cpp_name.as_posix()
        modified_cpp_name = pathlib.Path(modified_cpp_name + '_mod.cpp')

        # TODO: Future work: Here we can include an intermediate step where either the cleaned code, so without
        #  comments, can be fed to e.g. ChatGPT to automatically convert to normal C++ code. For now this file should
        #  be supplied upfront.

        # modified_cpp_name = pathlib.Path('data/test_cpp.cpp')
        # modified_cpp_name = pathlib.Path('input/test1/source_mod.cpp')
        # modified_cpp_name = pathlib.Path('input/nioz/test_cpp.cpp')
        # modified_cpp_name = pathlib.Path('input/mergesort/merge_sort_standard.cpp')
        # modified_cpp_name = pathlib.Path('input/multiplyvector/multiply_vector_standard.cpp')
        print("Loading standardized C++ file")
        modified_cpp_name = pathlib.Path('input/baseline/base_line_standard.cpp')


        with open("config.yaml", "r") as yaml_file:
            converted_cpp_file_path = config["input"]["converted_cpp_file_path"]

            if converted_cpp_file_path is None or converted_cpp_file_path == '':
                raise Exception("Converted C++ file path was not specified in config.yaml")

            modified_cpp_name = pathlib.Path(converted_cpp_file_path)

        # cppModifier.write_cpp_file(stripped_cpp, modified_cpp_name)

        mod_path = pathlib.Path(modified_cpp_name)

        functions = cppModifier.extract_cpp_functions(mod_path, llvm_path)
        print("Extracted C++ functions:")
        print(functions)

        function_calls = rModifier.find_function_calls(functions, file_path_input)
        print("Found function calls in R file")

        template_path = pathlib.Path('data/templates/CSVTemplate.cpp')
        if use_json:
            template_path = pathlib.Path('data/templates/JSONTemplate.cpp')
            print("Using JSON to generate new code")

        cppModifier.create_api(modified_cpp_name, template_path, functions, folder, use_json)
        print("Created C++ API with functions")
        rModifier.replace_function_calls(function_calls, functions, file_path_input, folder, use_json)
        print("Replaced function calls in R file")

        # Containerize the R file
        if containerize_R:
            rContainerizer.containerize_r_file(pathlib.Path(f'output/{folder}/modified.R'), folder)
            print("Containerized R file")

        end_time = time.time()

        print("Containerization process completed!")

        run_time = end_time - start_time
        print(f"Total execution time: {run_time}")


# program('data/05_IntegratedPPems_small.R')
# program('input/nioz/nioz_r.R')
# program('input/mergesort/merge_sort.R')
# program('input/multiplyvector/multiply_vector.R')
program()
# program('input/test1/source_r.R')

# program_temp("data/test_cpp.cpp")

# Press the green button in the gutter to run the script.
# packageDetector.find_used_supported_packages(pathlib.Path('data/05_IntegratedPPems_small.R'))
# analyze_notebook('data/05_IntegratedPPems_small.ipynb')
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
