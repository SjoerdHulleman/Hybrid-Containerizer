# Requirements
This Python application requires LLVM to be installed on your machine. The application is confirmed to be compatible with version 19.1.0, so we recommend installing this version.

Download link: https://github.com/llvm/llvm-project/releases/tag/llvmorg-19.1.0

After installation, please specify the folder containing the LLVM installation in `configuration.yaml`.

To install all dependencies:
```
pip install -r requirements.txt
```

If you want to generate container files for R, set `containerize_R` to `True`. Then also make sure the NaaVRE-containerizer-service is running. This can be found at https://github.com/NaaVRE/NaaVRE-containerizer-service. Set up the correct configuration according to the docs of the service, and use the following command to run (for local use):
```
 docker run --env-file dev.env -p 127.0.0.1:8000:8000 naavre-containerizer-service:dev
```

## Usage of the Hybrid Containerizer
- Put your script and C++ files in a folder within the input folder.
- For R with Rcpp:
    - Include the original Rcpp C++ file
    - Include the C++ file with the Rcpp code converted to standard C++ (`converted_cpp_file_path` in the configuration)
- Set the correct filepaths in the configurations
- Run `main.py`
- The modified code, including C++ container files, will be placed in the `output` folder
- The R container files will be placed in the Git repository set in the NaaVRE-containerizer-service settings

## Using the output files
To build a C++/R image, use the following command:
```
docker build -t IMAGE_NAME .
```
To run a C++ container, use the following command:
```
docker run --rm --network host IMAGE_NAME
```

To run an R container, use the following command:
```
docker run --rm --network host IMAGE_NAME -c "source /venv/bin/activate; Rscript /app/task.R"
```
If you are running the version with separate R and C++ containers, make sure the C++ container is already running since this container functions as an API for the R container.

These commands apply if the containers are running on the same machine. If this is not the case, other network settings apply.
