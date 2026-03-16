## Getting started

- install the required packages
```bash
pip install bs4 lxml
```
- set in the `cb_driver.py` file the following paths:
```python
CDSS_CLI_PATH = "" # <-- put the path of the CDSS binary CLI file
DRUG_FILES_DIR = "" # <-- put the path of the directory with the drugfiles
LANGUAGE_FILES_DIR = "" <-- # put the path of the directory with the translations
```
- since the HTML template directory is currently hardcoded, create a symbolic link in the current path pointing to that directory
- set in the `cb_driver.py` file the path of the directory that will contain the generated TQF files and the one for the generated HTML files:
```python
TQF_DIR_PATH = ""  # <-- put the path of the generated TQF files
OUT_DIR_PATH =  "" # <-- put the path of the generated HTML files
```
- the `CHECK_TYPICAL_RANGE_ONLY` flag allows to not only explore inside/outside the typical range, but also to check values between the typical and the limit range (WARNING: it will explode the number of tests to be performed!)
- run the script by passing as command line parameters
	+ the path of the CDSS core configuration file (the one containing the boundaries for the covariate values)
	+ the path of the template TQF file --- one is already provided in this directory for tuberculosis
