import json

from data.languages import SUPPORTED_LANGUAGES
from pathlib import Path


class ProgrammingLanguage:
    def __init__(self, language: str, notebook: bool, fileExtension: str, filePath: Path, supported: bool):
        self.language = language
        self.notebook = notebook
        self.fileExtension = fileExtension
        self.path = filePath
        self.supported = supported

    def __repr__(self):
        return ("Language: %s \n"
                "File extension: %s \n"
                "Path: %s \n"
                "Supported: %s") % (self.language, self.fileExtension, self.path, self.supported)


def detect_langauge(filePath: Path) -> ProgrammingLanguage:
    # Layer 1: Look at extension
    pl = detect_extension(filePath)

    # Layer 1.1: Check Jupyter Notebook language
    if pl.notebook:
        pl.language = analyze_notebook(filePath)
        if pl.language in SUPPORTED_LANGUAGES:
            pl.supported = True

    return pl


def detect_extension(filePath: Path) -> ProgrammingLanguage:
    suffix = filePath.suffix

    result = ProgrammingLanguage("", False, suffix, filePath, False)

    match suffix:
        case ".ipynb":
            result.language = "Unknown"
            result.notebook = True

        # In the future also include for rmd files
        case ".R":
            result.language = "R"

        case ".rdata" | ".rhistory" | ".rds" | ".rda":
            result.language = 'R data'

        case _:
            result.language = "N/A"

    if result.language in SUPPORTED_LANGUAGES:
        result.supported = True

    return result


# def detect_libraries(filePath):


# Analyze Jupyter Notebooks, as these have a JSON file structure
def analyze_notebook(filepath: Path) -> str:
    with open(filepath, 'r', encoding='utf-8') as file:
        data = json.load(file)
        metadata = data['metadata']
        # print(metadata)

        kernelspec = metadata['kernelspec']
        language = kernelspec['language']
        # print(kernelspec)
        return language
        # for cell in data.get('cells', []):
        #     # print('-----')
        #     if cell['cell_type'] == 'code':
        #         cell_contents = ''.join(cell['source'])
        #         # print(cell_contents)
