# SPDX-License-Identifier: AGPL-3.0-or-later
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Runs tucuxi_cdss_cli several times on one .tqf to exercise CLI input handling.
"""

__author__ = "Yann Thoma"
__copyright__ = "Copyright 2024, REDS institute --- HEIG-VD"
__maintainer__ = "Yann Thoma"
__contact__ = "yann.thoma@heig-vd.ch"

import argparse
import logging
import os
import shutil
import subprocess
from dataclasses import dataclass
from typing import List, Optional


LOGFILE_NAME = "000_LOG.txt"

DEFAULT_CDSS_CLI_PATH = "../../../build/debug/main/tucuxi_cdss_cli"
DEFAULT_DRUG_FILES_DIR = "../../../../tucuxi-drugs/drugfiles/"
DEFAULT_LANGUAGE_FILES_DIR = "../../../language/"
DEFAULT_TEMPLATE_DIR = "../../../build/debug/main/html_template/"
DEFAULT_TEMPLATE_NAME = "template"
DEFAULT_OUT_DIR = "output"
DEFAULT_TQF_NAME = "imatinib_correct_sample.tqf"
DEFAULT_CONFIG_PATH = os.path.abspath(
    os.path.join(os.path.dirname(os.path.abspath(__file__)), "../../../config.xml")
)


# Logger configuration.
logging.basicConfig(level=logging.DEBUG)
logger_blocklist = ["watchdog", "matplotlib", "PIL"]
for module in logger_blocklist:
    logging.getLogger(module).setLevel(logging.WARNING)


def parse_args():
    p = argparse.ArgumentParser(
        description=(
            "Run tucuxi_cdss_cli several times with one .tqf file to cover "
            "CLI input-error management paths."
        )
    )
    p.add_argument(
        "--config-fpath",
        default=DEFAULT_CONFIG_PATH,
        help="Path to config.xml (default: repository root config.xml)",
    )
    p.add_argument("--tqf-name", default=DEFAULT_TQF_NAME, help="Name of the .tqf file to use")

    p.add_argument("--cdss-cli", default=DEFAULT_CDSS_CLI_PATH, help="Path to tucuxi_cdss_cli")
    p.add_argument("--drug-files-dir", default=DEFAULT_DRUG_FILES_DIR, help="Path to drugfiles directory")
    p.add_argument("--language-files-dir", default=DEFAULT_LANGUAGE_FILES_DIR, help="Path to language directory")
    p.add_argument("--template-dir", default=DEFAULT_TEMPLATE_DIR, help="Path to html_template directory")
    p.add_argument("--template-name", default=DEFAULT_TEMPLATE_NAME, help="Template name")
    p.add_argument("--out-dir", default=DEFAULT_OUT_DIR, help="Output directory for generated HTML")

    p.add_argument("--keep-output", action="store_true", help="Do not delete output dir before running")
    p.add_argument("--verbose-cli", action="store_true", help="Print CLI stdout/stderr even on success")
    return p.parse_args()


@dataclass
class Scenario:
    name: str
    description: str
    extra_args: List[str]
    expected_return_code: int
    expect_stdout: Optional[str] = None
    expect_stderr: Optional[str] = None
    add_output_arg: bool = True
    precreate_output_dir: bool = True


def _run_command(command: List[str]):
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    return process.returncode, stdout.decode(errors="replace"), stderr.decode(errors="replace")


def _build_scenarios(args, tqf_abs: str):
    missing_file = os.path.join(args.tqfs_path, "file_does_not_exist.tqf")
    missing_dir = os.path.join(args.tqfs_path, "dir_does_not_exist")
    partially_correct_tqf = os.path.join(args.tqfs_path, "imatinib_partially_correct.tqf")
    import_error_tqf = os.path.join(args.tqfs_path, "imatinib_import_error.tqf")
    uncorrect_tqf = os.path.join(args.tqfs_path, "imatinib_uncorrect.tqf")

    base_args = [
        "-d", os.path.abspath(args.drug_files_dir),
        "-l", os.path.abspath(args.language_files_dir),
        "-i", tqf_abs,
        "-c", os.path.abspath(args.config_fpath),
        "-t", args.template_name,
        "-p", os.path.abspath(args.template_dir),
        "-j",
    ]

    success_code = 0
    partial_success_code = 1
    no_success_code = 2
    bad_arguments_code = 3
    import_error_code = 4

    return [
        Scenario(
            name="help",
            description="Help command",
            extra_args=["--help"],
            expected_return_code=success_code,
            expect_stdout="Print this help message",
            add_output_arg=False,
        ),
        Scenario(
            name="invalid_jsondump_value",
            description="Invalid bool value for -j triggers option parsing error",
            extra_args=["--jsondump=notabool"],
            expected_return_code=bad_arguments_code,
            add_output_arg=False,
        ),
        Scenario(
            name="baseline_valid",
            description="Reference execution with valid arguments",
            extra_args=base_args,
            expected_return_code=success_code,
        ),
        Scenario(
            name="partially_correct_input",
            description="Run imatinib_partially_correct.tqf with valid arguments",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", partially_correct_tqf,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=partial_success_code,
        ),
        Scenario(
            name="import_error_input",
            description="Run imatinib_import_error.tqf with valid arguments",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", import_error_tqf,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=import_error_code,
        ),
        Scenario(
            name="uncorrect_input",
            description="Run imatinib_uncorrect.tqf with valid arguments",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", uncorrect_tqf,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=no_success_code,
        ),
        Scenario(
            name="missing_drugpath",
            description="Missing mandatory -d",
            extra_args=base_args[2:],
            expected_return_code=bad_arguments_code,
            expect_stdout="The drug model files directory is mandatory",
        ),
        Scenario(
            name="invalid_drugpath",
            description="Non-existent path for -d",
            extra_args=["-d", missing_dir] + base_args[2:],
            expected_return_code=bad_arguments_code,
            expect_stdout="is missing",
        ),
        Scenario(
            name="missing_input",
            description="Missing mandatory -i",
            extra_args=["-d", os.path.abspath(args.drug_files_dir)] + base_args[6:],
            expected_return_code=bad_arguments_code,
            expect_stdout="The input query file is mandatory",
        ),
        Scenario(
            name="invalid_input_file",
            description="Non-existent path for -i",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", missing_file,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=bad_arguments_code,
            expect_stdout="The specified input query file",
        ),
        Scenario(
            name="missing_config_uses_default",
            description="Missing optional -c uses default ./config.xml",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", tqf_abs,
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=success_code,
            expect_stdout="Configuration file not specified --- set by default to ./config.xml",
        ),
        Scenario(
            name="missing_outputpath",
            description="Missing mandatory -o",
            extra_args=base_args,
            expected_return_code=bad_arguments_code,
            expect_stdout="The output directory path is mandatory",
            add_output_arg=False,
        ),
        Scenario(
            name="missing_output_dir_is_created",
            description="Non-existent -o path is created automatically",
            extra_args=base_args,
            expected_return_code=success_code,
            expect_stdout="is not existent, it will be created automatically",
            precreate_output_dir=False,
        ),
        Scenario(
            name="invalid_config",
            description="Non-existent path for -c",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", tqf_abs,
                "-c", missing_file,
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=bad_arguments_code,
            expect_stdout="The configuration file",
        ),
        Scenario(
            name="missing_template_name",
            description="Missing mandatory -t",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", tqf_abs,
                "-c", os.path.abspath(args.config_fpath),
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=bad_arguments_code,
            expect_stdout="The HTML template name is mandatory",
        ),
        Scenario(
            name="invalid_template_path",
            description="Non-existent path for -p",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", tqf_abs,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", missing_dir,
                "-j",
            ],
            expected_return_code=bad_arguments_code,
            expect_stdout="HTML template directory",
        ),
        Scenario(
            name="missing_template_path_uses_default",
            description="Missing optional -p uses default template directory",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", tqf_abs,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-j",
            ],
            expected_return_code=success_code,
            expect_stdout="The HTML template directory has not been specified --- defaulting to",
        ),
        Scenario(
            name="invalid_template_name",
            description="Non-existent template file for -t",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", tqf_abs,
                "-c", os.path.abspath(args.config_fpath),
                "-t", "template_does_not_exist",
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=bad_arguments_code,
            expect_stdout="The selected HTML template",
        ),
        Scenario(
            name="invalid_language_path",
            description="Non-existent path for -l",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", missing_dir,
                "-i", tqf_abs,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=bad_arguments_code,
            expect_stdout="The language files directory",
        ),
        Scenario(
            name="missing_language_path_uses_default",
            description="Missing optional -l uses default language directory",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-i", tqf_abs,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-j",
            ],
            expected_return_code=success_code,
            expect_stdout="The language files directory has not been specified ---  defaulting to",
        ),
        Scenario(
            name="explicit_language",
            description="Non-empty -g value is accepted",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", tqf_abs,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-g", "FR",
                "-j",
            ],
            expected_return_code=success_code,
        ),
        Scenario(
            name="empty_language",
            description="Empty value for -g",
            extra_args=[
                "-d", os.path.abspath(args.drug_files_dir),
                "-l", os.path.abspath(args.language_files_dir),
                "-i", tqf_abs,
                "-c", os.path.abspath(args.config_fpath),
                "-t", args.template_name,
                "-p", os.path.abspath(args.template_dir),
                "-g", "",
                "-j",
            ],
            expected_return_code=bad_arguments_code,
            expect_stdout="The desired language is empty",
        ),
    ]


def _main(args):
    out_dir = os.path.abspath(args.out_dir)
    tqf_abs = os.path.join(args.tqfs_path, args.tqf_name)
    scenarios = _build_scenarios(args, tqf_abs)

    passed = 0
    with open(os.path.join(out_dir, LOGFILE_NAME), "wt", encoding="utf-8") as f:
        f.write("Execution started\n")

    for scenario in scenarios:
        scenario_out = os.path.join(out_dir, scenario.name)
        if scenario.precreate_output_dir:
            os.makedirs(scenario_out, exist_ok=True)

        command = [os.path.abspath(args.cdss_cli)] + scenario.extra_args
        if scenario.add_output_arg:
            command += ["-o", scenario_out]
        logging.info("[%s] %s", scenario.name, scenario.description)
        logging.info("[%s] Command: %s", scenario.name, " ".join(command))

        returncode, stdout, stderr = _run_command(command)
        match_returncode = returncode == scenario.expected_return_code

        match_stdout = True
        match_stderr = True
        if scenario.expect_stdout:
            match_stdout = scenario.expect_stdout in stdout
        if scenario.expect_stderr:
            match_stderr = scenario.expect_stderr in stderr

        outcome_ok = match_returncode and match_stdout and match_stderr
        if outcome_ok:
            passed += 1

        with open(os.path.join(out_dir, LOGFILE_NAME), "at", encoding="utf-8") as f:
            f.write(f"[{scenario.name}] returncode={returncode} expected={scenario.expected_return_code} ")
            f.write(f"result={'PASS' if outcome_ok else 'FAIL'}\n")

        if args.verbose_cli or not outcome_ok:
            if stdout:
                logging.info("[%s] --- stdout ---\n%s", scenario.name, stdout)
            if stderr:
                level = logging.WARNING if outcome_ok else logging.ERROR
                logging.log(level, "[%s] --- stderr ---\n%s", scenario.name, stderr)

    with open(os.path.join(out_dir, LOGFILE_NAME), "at", encoding="utf-8") as f:
        f.write(f"Execution complete: {passed}/{len(scenarios)} scenarios passed\n")


if __name__ == "__main__":
    args = parse_args()
    args.tqfs_path = os.path.dirname(os.path.abspath(__file__))

    if not os.path.isfile(args.config_fpath):
        raise FileNotFoundError(f"Invalid config file: {args.config_fpath}")
    tqf_abs = os.path.join(args.tqfs_path, args.tqf_name)
    if not os.path.isfile(tqf_abs):
        raise FileNotFoundError(f"Missing TQF file: {tqf_abs}")
    if not os.path.isfile(args.cdss_cli):
        raise FileNotFoundError(f"Missing CDSS CLI binary: {args.cdss_cli}")

    if (not args.keep_output) and os.path.isdir(args.out_dir):
        shutil.rmtree(args.out_dir)
    os.makedirs(args.out_dir, exist_ok=True)

    _main(args)
