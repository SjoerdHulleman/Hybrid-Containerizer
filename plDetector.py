import json
import pathlib

from data.languages import SUPPORTED_LANGUAGES


# Present this file with the fileContents
# def detect_language(filePath):

class ProgrammingLanguage:
    def __init__(self, language, fileExtension, filePath, supported):
        self.language = language
        self.fileExtension = fileExtension
        self.path = filePath
        self.supported = supported

    def __repr__(self):
        return ("Language: %s \n"
                "File extension: %s \n"
                "Path: %s \n"
                "Supported: %s") % (self.language, self.fileExtension, self.path, self.supported)


def detect_extension(filePath):
    path = pathlib.Path(filePath)
    suffix = path.suffix

    result = ProgrammingLanguage("", suffix, path, False)

    match suffix:
        case ".ipynb":
            result.language = analyze_notebook(filePath)

        case ".r":
            result.language = "R"

        case ".rdata" | ".rhistory" | ".rds" | ".rda":
            result.language = 'R data'

        case _:
            result.language = "N/A"

    if result.language in SUPPORTED_LANGUAGES:
        result.supported = True

    print(result)


# def detect_libraries(filePath):


# Analyze Jupyter Notebooks, as these have a JSON file structure
def analyze_notebook(filepath):
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
