import pathlib
import nbformat
import requests
import json


def convert_r_to_notebook(filePath: pathlib.Path, folder: str):
    with open (filePath) as file:
        code = file.read()
        notebook = nbformat.v4.new_notebook()
        notebook['cells'] = [nbformat.v4.new_code_cell(code)]

        output_path = f'output/{folder}/modified.ipynb'

        with open(output_path, 'w') as newFile:
            nbformat.write(notebook, newFile)
            newFile.close()

        file.close()


def containerize_r_file(r_file_path: pathlib.Path, folder: str):
    # Convert to a Jupyter Notebook file since the used API needs this format
    convert_r_to_notebook(r_file_path, folder)
    notebook_path = pathlib.Path(f'output/{folder}/modified.ipynb')

    with open(notebook_path) as notebook_file:
        notebook = nbformat.read(notebook_file, as_version=4)

        # Create request body to extract cell data from Jupyter Notebook
        extract_cell_data = {
            "data": {
                "cell_index": 0,
                "kernel": "IRkernel",
                "notebook": notebook,
                "save": True,
                "base_image_name": "r"
            }
        }

        # Set up correct endpoint
        base_url = "http://localhost:8000"
        extract_cell_url = f"{base_url}/extract_cell"
        fake_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjkwMDAwMDAwMDAsImlhdCI6MTcyODk4MTczOCwiYXV0aF90aW1lIjoxNzI4OTgxNzM4LCJqdGkiOiJjZWFlYjlhMC0zZjQxLTRiZmEtYTgzMS02OTdmYjUzNzdlMTAiLCJpc3MiOiJodHRwczovL25hYXZyZS1kZXYudGVzdC9hdXRoL3JlYWxtcy92cmUiLCJhdWQiOiJhY2NvdW50Iiwic3ViIjoiMDAwMDAwMDAtMDAwMC0wMDAwLTAwMDAtMDAwMDAwMDAwMDAwIiwidHlwIjoiQmVhcmVyIiwiYXpwIjoibmFhdnJlIiwic2Vzc2lvbl9zdGF0ZSI6IjQ4NmJmM2U3LTIyYzItNDQyOC04Nzk4LTU5ZmYzMjI1YjdhZiIsImFjciI6IjEiLCJyZWFsbV9hY2Nlc3MiOnsicm9sZXMiOlsiZGVmYXVsdC1yb2xlcy12cmUiLCJvZmZsaW5lX2FjY2VzcyIsInVtYV9hdXRob3JpemF0aW9uIl19LCJyZXNvdXJjZV9hY2Nlc3MiOnsiYWNjb3VudCI6eyJyb2xlcyI6WyJtYW5hZ2UtYWNjb3VudCIsIm1hbmFnZS1hY2NvdW50LWxpbmtzIiwidmlldy1wcm9maWxlIl19fSwic2NvcGUiOiJvcGVuaWQgcHJvZmlsZSBlbWFpbCIsInNpZCI6ImZlMTlmNjc0LTMwNjgtNGUwOC05N2VkLTVmNTRjYWE5OGVhNCIsImVtYWlsX3ZlcmlmaWVkIjpmYWxzZSwiZ3JvdXBzIjpbInVzZXJzIl0sInByZWZlcnJlZF91c2VybmFtZSI6InRlc3QtdXNlci0yIn0.yWYUvpVsMzCL9YOT3zPbul-OmASPiS5hgBcGS37UJi0"
        headers = {
            "Authorization": f"Bearer {fake_token}",
            "Content-Type": "application/json"
        }

        extract_cell_req = requests.post(url=extract_cell_url, json=extract_cell_data, headers=headers)
        containerize_url = f"{base_url}/containerize"
        containerize_data = {
            "cell": extract_cell_req.json()
        }
        try:
            containerize_req = requests.post(url=containerize_url, json=containerize_data, headers=headers)
        except:
            print("Something went wrong while containerizing your R file, please check your ouput Repo to check if "
                  "creation of the output files is successful.")
        print(json.dumps(containerize_req.json()))

        containerize_out_json = containerize_req.json()
        print(f"Containerization of R file completed, please find the workflow at: {containerize_out_json["workflow_url"]}"
              f". The created files can be found in your output repository.")


        # Print statements for testing
        # print(extract_cell_data)
        # print(json.dumps(extract_cell_data, indent=2))
        # print(json.dumps(extract_cell_req.json(), indent=2))
        # print(json.dumps(containerize_data, indent=2))
        # print(json.dumps(containerize_req.json(), indent=2))
