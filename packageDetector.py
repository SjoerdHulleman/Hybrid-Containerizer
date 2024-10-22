import subprocess

import plDetector
from data.languages import SUPPORTED_R_PACKAGES
from plDetector import ProgrammingLanguage
from pathlib import Path


# Find all packages in the provided R file
def find_R_packages(filePath: Path):
    # Find R packages through subprocess that runs an R file
    command = [
        'Rscript', '-e',
        f'renv::dependencies("{filePath.as_posix()}")'
    ]
    result = subprocess.run(command, capture_output=True, text=True)
    print(result)
    lines = result.stdout.splitlines()

    # Set to ensure no duplicate packages
    packages = set()

    package_line = False
    for line in lines:
        # After this line the packages are listed
        if not package_line:
            if 'Package' in line:
                package_line = True
                continue
            else:
                continue

        if package_line:
            line_parts = line.split()
            packages.add(line_parts[1])

    return packages


# Find all supported and used packages in the provided language or filepath
def find_used_supported_packages(input):
    # Check if we got a filepath of a ProgrammingLanguage object
    if isinstance(input, Path):
        language = plDetector.detect_langauge(input)
    elif isinstance(input, str):
        language = plDetector.detect_langauge(Path(input))
    elif isinstance(input, ProgrammingLanguage):
        language = input
    else:
        raise ValueError("Unsupported input: {}".format(input))

    used_supported_packages = set()

    match language.language:
        case 'R':
            packages = find_R_packages(language.path)

            print('Found the following packages for R:')

            for supported_package in SUPPORTED_R_PACKAGES:
                if supported_package in packages:
                    used_supported_packages.add(supported_package)
        case _:
            print('Unsupported language')

    print('Of which the following are supported:')
    print(used_supported_packages)
    return used_supported_packages
