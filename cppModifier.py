
from plDetector import ProgrammingLanguage
import re
import pathlib

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
        rcpp_export_regex = re.compile(r'\s*//\s*\[\[\s*Rcpp::export\s*\]\]\s*\n*\s*')
        comment_regex = re.compile(r'\s*//\s*.*\n*')

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