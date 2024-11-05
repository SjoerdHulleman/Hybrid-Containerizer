# This is a sample Python script.
import os
import pathlib

import cppModifier
import packageDetector
import plDetector
# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

from plDetector import analyze_notebook, detect_extension
from packageDetector import find_R_packages

import subprocess
import yaml


def print_hi(name):
    # Use a breakpoint in the code line below to debug your script.
    print(f'Hi, {name}')  # Press Ctrl+F8 to toggle the breakpoint.


def convert_rmd():
    command = "knitr::purl('data/05_IntegratedPPems_small.Rmd', output='data/05_IntegratedPPems_small.R')"
    result = subprocess.run(["Rscript", "-e", command], capture_output=True, text=True)
    print(result)


# def program_R(pl, packages):
#     if 'Rcpp' in packages:


def program(file_path_input):
    path = pathlib.Path(file_path_input)
    llvm_path = ''

    with open("config.yaml", "r") as yaml_file:
        config = yaml.safe_load(yaml_file)
        llvm_path = config["environment"]["llvm_path"]

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

        cppModifier.write_cpp_file(stripped_cpp, modified_cpp_name)

        mod_path = pathlib.Path(modified_cpp_name)

        cppModifier.extract_cpp_functions(mod_path, llvm_path)


program('data/05_IntegratedPPems_small.R')

# Press the green button in the gutter to run the script.
# packageDetector.find_used_supported_packages(pathlib.Path('data/05_IntegratedPPems_small.R'))
# analyze_notebook('data/05_IntegratedPPems_small.ipynb')
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
