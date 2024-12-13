# This is a sample Python script.
import os
import pathlib

import cppModifier
import packageDetector
import plDetector
import rModifier
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


def convert_r_to_notebook(filePath: pathlib.Path):
    with open (filePath) as file:
        code = file.read()
        notebook = nbformat.v4.new_notebook()
        notebook['cells'] = [nbformat.v4.new_code_cell(code)]

        output_path = 'output/modified.ipynb'

        with open(output_path, 'w') as newFile:
            nbformat.write(notebook, newFile)
            newFile.close()

        file.close()



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


def program(file_path_input):
    path = pathlib.Path(file_path_input)
    llvm_path = ''
    use_json = False

    with open("config.yaml", "r") as yaml_file:
        config = yaml.safe_load(yaml_file)
        llvm_path = config["environment"]["llvm_path"]
        use_json = config["settings"]["use_json"]

    if llvm_path is None or llvm_path == '':
        raise Exception("LLVM path is not specified in config.yaml")

    # Check which programming language we have
    pl = plDetector.detect_langauge(path)

    # Feed to the package detector to find supported packages
    used_supported_packages = packageDetector.find_used_supported_packages(pl)

    if pl.language == 'R':
        # program_R(pl, used_supported_packages)
        cpp_file_path = cppModifier.find_cpp_file(pl)
        stripped_cpp = cppModifier.strip_comments(cpp_file_path)

        modified_cpp_name = cpp_file_path.with_suffix('')
        modified_cpp_name = modified_cpp_name.as_posix()
        modified_cpp_name = pathlib.Path(modified_cpp_name + '_mod.cpp')

        modified_cpp_name = pathlib.Path('data/test_cpp.cpp')

        # cppModifier.write_cpp_file(stripped_cpp, modified_cpp_name)

        mod_path = pathlib.Path(modified_cpp_name)

        functions = cppModifier.extract_cpp_functions(mod_path, llvm_path)
        # print(functions)

        function_calls = rModifier.find_function_calls(functions, file_path_input)
        print(function_calls)

        template_path = pathlib.Path('data/templates/CSVTemplate.cpp')
        if use_json:
            template_path = pathlib.Path('data/templates/JSONTemplate.cpp')

        cppModifier.create_api(modified_cpp_name, template_path, functions, use_json)
        rModifier.replace_function_calls(function_calls, functions, file_path_input, True)


        convert_r_to_notebook('output/modified.R')

        notebook_path = pathlib.Path('output/modified.ipynb')

        with open(notebook_path) as notebook_file:
            notebook = nbformat.read(notebook_file, as_version=4)

            extract_cell_data = {
                "data": {
                    "cell_index": 0,
                    "kernel": "IRkernel",
                    "notebook": notebook,
                    "save": True,
                    "base_image_name": "r"
                }
            }

            print(extract_cell_data)

            extract_cell_url = "http://localhost:8000/extract_cell"

            fake_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjkwMDAwMDAwMDAsImlhdCI6MTcyODk4MTczOCwiYXV0aF90aW1lIjoxNzI4OTgxNzM4LCJqdGkiOiJjZWFlYjlhMC0zZjQxLTRiZmEtYTgzMS02OTdmYjUzNzdlMTAiLCJpc3MiOiJodHRwczovL25hYXZyZS1kZXYudGVzdC9hdXRoL3JlYWxtcy92cmUiLCJhdWQiOiJhY2NvdW50Iiwic3ViIjoiMDAwMDAwMDAtMDAwMC0wMDAwLTAwMDAtMDAwMDAwMDAwMDAwIiwidHlwIjoiQmVhcmVyIiwiYXpwIjoibmFhdnJlIiwic2Vzc2lvbl9zdGF0ZSI6IjQ4NmJmM2U3LTIyYzItNDQyOC04Nzk4LTU5ZmYzMjI1YjdhZiIsImFjciI6IjEiLCJyZWFsbV9hY2Nlc3MiOnsicm9sZXMiOlsiZGVmYXVsdC1yb2xlcy12cmUiLCJvZmZsaW5lX2FjY2VzcyIsInVtYV9hdXRob3JpemF0aW9uIl19LCJyZXNvdXJjZV9hY2Nlc3MiOnsiYWNjb3VudCI6eyJyb2xlcyI6WyJtYW5hZ2UtYWNjb3VudCIsIm1hbmFnZS1hY2NvdW50LWxpbmtzIiwidmlldy1wcm9maWxlIl19fSwic2NvcGUiOiJvcGVuaWQgcHJvZmlsZSBlbWFpbCIsInNpZCI6ImZlMTlmNjc0LTMwNjgtNGUwOC05N2VkLTVmNTRjYWE5OGVhNCIsImVtYWlsX3ZlcmlmaWVkIjpmYWxzZSwiZ3JvdXBzIjpbInVzZXJzIl0sInByZWZlcnJlZF91c2VybmFtZSI6InRlc3QtdXNlci0yIn0.yWYUvpVsMzCL9YOT3zPbul-OmASPiS5hgBcGS37UJi0"
            headers = {
                "Authorization": f"Bearer {fake_token}",
                "Content-Type": "application/json"
            }

            # print(json.dumps(extract_cell_data, indent=2))

            extract_cell_req = requests.post(url=extract_cell_url, json=extract_cell_data, headers=headers)


            # print(json.dumps(extract_cell_req.json(), indent=2))

            containerize_url = "http://localhost:8000/containerize"

            containerize_data = {
                "cell": extract_cell_req.json()
            }

            print(json.dumps(containerize_data, indent=2))

            containerize_req = requests.post(url=containerize_url, json=containerize_data, headers=headers)

            #print(json.dumps(containerize_req.json(), indent=2))


        # with open('/output/modified.R') as r_file:
        #     data = r_file.read()
        #     url = "http://localhost:8000/containerize"
        #
        #     params = {
        #         "cell": {
        #             "title": "Test",
        #             "base_container_image": {
        #                 "build": "ghcr.io/qcdis/naavre/naavre-cell-build-r:v0.18",
        #                 "runtime": "ghcr.io/qcdis/naavre/naavre-cell-runtime-r:v0.18"
        #             },
        #             "inputs": [],
        #             "outputs": [],
        #             "params": [],
        #             "secrets": [],
        #             "confs": [],
        #             "dependencies": [],
        #             "chart_obj": {},
        #             "kernel": "IRkernel",
        #             "original_source": data
        #         }
        #     }


program('data/05_IntegratedPPems_small.R')

# program_temp("data/test_cpp.cpp")

# Press the green button in the gutter to run the script.
# packageDetector.find_used_supported_packages(pathlib.Path('data/05_IntegratedPPems_small.R'))
# analyze_notebook('data/05_IntegratedPPems_small.ipynb')
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
