# SPDX-License-Identifier: AGPL-3.0-or-later
import argparse
from email.mime import base
import logging
import os
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET
from copy import copy
from cell_modifiers import CellModifier, E

TMP_TQF_DIR = "tmptqf"

index = 0
index_cfg = 0

# Logger configuration.
logging.basicConfig(level=logging.INFO)

def find_matching_file(directory):
    matches = []
    for filename in os.listdir(directory):
        if re.fullmatch(r"imatinib_\d+_\d+-\d+-\d+_\d+h\d+m\d+s\.html", filename):
            path = os.path.join(directory, filename)
            if os.path.isfile(path):
                matches.append(path)
    if not matches:
        return None
    return max(matches, key=os.path.getmtime)

def _run_one(args, suffix=""):
    """
    Run an individual execution of the CDSS CLI with the generated (fuzzed)
    config/input files.
    """
    command = [
        args.cdss_cli_path,
        "-d", args.drug_files_dir,
        "-l", args.language_files_dir,
        "-i", args.input_file,
        "-c", args.config_file,
        "-t", args.template_name,
        "-p", args.template_dir,
        "-o", args.out_dir_path,
    ]

    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    logging.debug("Command to execute: " + " ".join(command))

    stdout, stderr = process.communicate()

    base = os.path.basename(args.original_input)
    output_name = base.replace(".tqf", f"{suffix}.html")
    output_path = os.path.join(args.out_dir_path, output_name)

    if process.returncode in (0, 1):
        expected_out_path = find_matching_file(args.out_dir_path)  # already a full path
        if expected_out_path is None:
            error = (
                f"EXECUTION FAILED [{process.returncode}]: "
                f"No HTML generated/found for file: {suffix}\n"
            )
            with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
                f.write(error)
            with open(os.path.join(args.out_dir_path, args.error_logfile), "at") as f:
                f.write(error)

            if stderr:
                with open(os.path.join(args.out_dir_path, args.crash_logfile), "at") as f:
                    f.write(error)
                    f.write(stderr.decode(errors="replace") + "\n")
            return

        os.rename(expected_out_path, output_path)

        if process.returncode == 1:
            with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
                f.write(
                    "Execution complete but some request failed at some stage of the process. "
                    f"Output saved as {output_name}\n"
                )
        else:
            with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
                f.write(f"Execution complete! Output saved as {output_name}\n")

    elif process.returncode in (2, 3):
        if process.returncode == 2:
            error = (
                f"EXECUTION FAILED [{process.returncode}]: No HTML generated for file: {suffix}. "
                "No request could be fully processed\n"
            )
        else:
            error = (
                f"EXECUTION FAILED [{process.returncode}]: No HTML generated for file: {suffix}. "
                "The query file could not be loaded.\n"
            )

        with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
            f.write(error)
        with open(os.path.join(args.out_dir_path, args.error_logfile), "at") as f:
            f.write(error)

    elif process.returncode == 255:  # actually -1
        error = (
            f"EXECUTION FAILED [{process.returncode}]: No HTML generated for file: {suffix}. "
            "Import error detected.\n"
        )
        with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
            f.write(error)
        with open(os.path.join(args.out_dir_path, args.error_logfile), "at") as f:
            f.write(error)

    elif stderr:
        error = (
            f"EXECUTION FAILED [{process.returncode}]: Program crashed during processing of file: {suffix}\n"
        )
        with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
            f.write(error)
        with open(os.path.join(args.out_dir_path, args.error_logfile), "at") as f:
            f.write(error)
        with open(os.path.join(args.out_dir_path, args.crash_logfile), "at") as f:
            f.write(error)
            f.write(stderr.decode(errors="replace") + "\n")

    else:
        error = (
            f"EXECUTION FAILED [{process.returncode}]: Unknown error during processing of file: {suffix}\n"
        )
        with open(os.path.join(args.out_dir_path, args.logfile_name), "at") as f:
            f.write(error)
        with open(os.path.join(args.out_dir_path, args.error_logfile), "at") as f:
            f.write(error)

def process_test(args, value_modifier, file_to_modify, file_prefix, attr_to_update):
    """
    Alter the given configuration file/input file and execute the CDSS CLI.
    """
    tree = ET.parse(file_to_modify)
    root = tree.getroot()

    def process_element(element):
        for child in element:
            if child.text is not None and child.text.strip() != "":
                try:
                    original = copy(child.text)
                    child.text = str(value_modifier(child.tag, child.text))
                    global index
                    index += 1
                    
                    modified_file = os.path.abspath(os.path.join(TMP_TQF_DIR, f"{file_prefix}_{index}.tqf"))
                    
                    tree.write(modified_file, encoding="utf-8", xml_declaration=True)

                    with open(
                        os.path.join(args.out_dir_path, args.logfile_name), "at"
                    ) as f:
                        f.write(f"File modified: {modified_file}\n")
                        f.write(f"Changed cell {child.tag} to {child.text}\n")

                    setattr(args, attr_to_update, modified_file)

                    suffix = f"_{file_prefix}_{index}"
                    _run_one(args, suffix)

                    with open(
                        os.path.join(args.out_dir_path, args.logfile_name), "at"
                    ) as f:
                        f.write("---------------\n")

                    child.text = str(original)
                except (ValueError, TypeError, E):
                    pass

            process_element(child)

    process_element(root)


def test_input(args, value_modifier):
    process_test(args, value_modifier, args.original_input, "imatinib", "input_file")


def test_config(args, value_modifier):
    process_test(args, value_modifier, args.original_config, "config", "config_file")


def parse_arguments():
    """
    Input arguments parsing.
    """
    parser = argparse.ArgumentParser(
        description="Configure global script variables.", add_help=True
    )

    parser.add_argument(
        "-i",
        "--original_input",
        type=str,
        default="imatinib.tqf",
        help="Path of the original input TQF (default: 'imatinib.tqf')",
    )
    parser.add_argument(
        "-f",
        "--original_config",
        type=str,
        default="config.xml",
        help="Path of the original configuration file (default: 'config.xml')",
    )
    parser.add_argument(
        "-o",
        "--out_dir_path",
        type=str,
        default="output",
        help="Path to the output directory (default: 'output')",
    )
    parser.add_argument(
        "-l",
        "--logfile_name",
        type=str,
        default="000_LOG.txt",
        help="Name of the log file (default: '000_LOG.txt')",
    )
    parser.add_argument(
        "-c",
        "--cdss_cli_path",
        type=str,
        default="../../tucuxi-cdss-core/build/tucuxi_cdss_cli",
        help="Path to the CDSS CLI executable (default: '../../tucuxi-cdss-core/build/tucuxi_cdss_cli')",
    )
    parser.add_argument(
        "-d",
        "--drug_files_dir",
        type=str,
        default="../../tucuxi-drugs/drugfiles/",
        help="Path to the drug files directory (default: '../../tucuxi-drugs/drugfiles/')",
    )
    parser.add_argument(
        "-g",
        "--language_files_dir",
        type=str,
        default="../../tucuxi-cdss-core/language/",
        help="Path to the language files directory (default: '../../tucuxi-cdss-core/language/')",
    )
    parser.add_argument(
        "-t",
        "--template_dir",
        type=str,
        default="../../tucuxi-cdss-core/html_template/",
        help="Path to the HTML templates directory (default: '../../tucuxi-cdss-core/html_template/')",
    )
    parser.add_argument(
        "-n",
        "--template_name",
        type=str,
        default="chuv_imatinib_full",
        help="Name of the template to use (default: 'chuv_imatinib_comp')",
    )
    parser.add_argument(
        "-e",
        "--expected_out_fname",
        type=str,
        default="imatinib_1_12-5-2023_0h0m0s.html",
        help="Expected output file name (default: 'imatinib_1_12-5-2023_0h0m0s.html')",
    )

    return parser.parse_args()


def get_unique_logfile_name(args):
    base_name, ext = os.path.splitext(args.logfile_name)

    counter = int(base_name.split("_")[0])
    prefix = "_".join(base_name.split("_")[1:])

    while os.path.exists(
        os.path.join(args.out_dir_path, f"{counter:03d}_{prefix}{ext}")
    ):
        counter += 1

    args.logfile_name = f"{counter:03d}_{prefix}{ext}"
    args.error_logfile = f"{counter:03d}_ERROR{ext}"
    args.crash_logfile = f"{counter:03d}_CRASH{ext}"


def valid_paths(args) -> bool:
    """
    Check that the given paths are valid/existing ones.

    Parameters
    ----------
    args: object carrying the paths given as parameters

    Returns
    -------
    True if all the given paths are valid/existing, False otherwise.
    The in
    """
    paths_valid = True

    if not os.path.isfile(args.original_input):
        logging.error("Invalid input TQF file: " + args.original_input)
        paths_valid = False
    if not os.path.isfile(args.original_config):
        logging.error(
            "Invalid original configuration file path: " + args.original_config
        )
        paths_valid = False
    if not os.path.isfile(args.cdss_cli_path):
        logging.error("Invalid CDSS CLI executable path: " + args.cdss_cli_path)
        paths_valid = False
    if not os.path.isdir(args.drug_files_dir):
        logging.error("Invalid drug files directory path: " + args.drug_files_dir)
        paths_valid = False
    if not os.path.isdir(args.language_files_dir):
        logging.error(
            "Invalid language files directory path: " + args.language_files_dir
        )
        paths_valid = False
    if not os.path.isdir(args.template_dir):
        logging.error("Invalid template directory path: " + args.template_dir)
        paths_valid = False
    template_path = os.path.join(args.template_dir, args.template_name)
    if not os.path.isdir(template_path):
        logging.error("Invalid template name: " + template_path)
        paths_valid = False

    return paths_valid


if __name__ == "__main__":
    args = parse_arguments()

    args.cdss_cli_path = os.path.abspath(args.cdss_cli_path)
    args.drug_files_dir = os.path.abspath(args.drug_files_dir)
    args.language_files_dir = os.path.abspath(args.language_files_dir)
    args.template_dir = os.path.abspath(args.template_dir)
    args.original_input = os.path.abspath(args.original_input)
    args.original_config = os.path.abspath(args.original_config)
    args.out_dir_path = os.path.abspath(args.out_dir_path)

    # Create the output directory (if missing).
    if not os.path.isdir(args.out_dir_path):
        os.makedirs(args.out_dir_path, exist_ok=False)
    get_unique_logfile_name(args)

    logging.info(f"INPUT_FILE: {args.original_input}")
    logging.info(f"CONFIG_FILE: {args.original_config}")
    logging.info(f"OUT_DIR_PATH: {args.out_dir_path}")
    logging.info(f"LOGFILE_NAME: {args.logfile_name}")
    logging.info(f"CDSS_CLI_PATH: {args.cdss_cli_path}")
    logging.info(f"DRUG_FILES_DIR: {args.drug_files_dir}")
    logging.info(f"LANGUAGE_FILES_DIR: {args.language_files_dir}")
    logging.info(f"TEMPLATE_DIR: {args.template_dir}")
    logging.info(f"TEMPLATE_NAME: {args.template_name}")
    logging.info(f"EXPECTED_OUT_FNAME: {args.expected_out_fname}")

    if not valid_paths(args):
        sys.exit(-1)

    # Create an empty tmp directory for TQF files.
    if os.path.isdir(TMP_TQF_DIR):
        shutil.rmtree(TMP_TQF_DIR)
    os.makedirs(TMP_TQF_DIR, exist_ok=False)

    # Retrieve the set of functions used to fuzz XML values.
    modifiers = [
        getattr(CellModifier, func)
        for func in dir(CellModifier)
        if callable(getattr(CellModifier, func)) and not func.startswith("__")
    ]

    cnt = 1
    for modifier in modifiers:
        modifier_str = str(modifier).split(".")[-1].split(" ")[0]
        logging.info(
            "## Test "
            + str(cnt)
            + " / "
            + str(len(modifiers))
            + " --- ["
            + modifier_str
            + "]"
        )

        args.config_file = args.original_config
        test_input(args, modifier)
        args.input_file = args.original_input
        test_config(args, modifier)
        cnt += 1
