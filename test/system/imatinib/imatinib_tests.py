# SPDX-License-Identifier: AGPL-3.0-or-later
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Runs the various .tqf requests available in the specified directory.
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
from typing import NoReturn


LOGFILE_NAME = "000_LOG.txt"

DEFAULT_CDSS_CLI_PATH = "../../../build/debug/main/tucuxi_cdss_cli"
DEFAULT_DRUG_FILES_DIR = "../../../../tucuxi-drugs/drugfiles/"
DEFAULT_LANGUAGE_FILES_DIR = "../../../language/"
DEFAULT_TEMPLATE_DIR = "../../../build/debug/main/html_template/"
DEFAULT_TEMPLATE_NAME = "template"
DEFAULT_OUT_DIR = "output"


# Logger configuration.
logging.basicConfig(level=logging.DEBUG)
logger_blocklist = ["watchdog", "matplotlib", "PIL"]
for module in logger_blocklist:
    logging.getLogger(module).setLevel(logging.WARNING)


def parse_args():
    p = argparse.ArgumentParser(
        description="Run all .tqf files in a directory with tucuxi_cdss_cli."
    )
    p.add_argument("config_fpath", help="Path to config.xml")
    p.add_argument("tqfs_path", help="Directory containing .tqf files")

    p.add_argument("--cdss-cli", default=DEFAULT_CDSS_CLI_PATH, help="Path to tucuxi_cdss_cli")
    p.add_argument("--drug-files-dir", default=DEFAULT_DRUG_FILES_DIR, help="Path to drugfiles directory")
    p.add_argument("--language-files-dir", default=DEFAULT_LANGUAGE_FILES_DIR, help="Path to language directory")
    p.add_argument("--template-dir", default=DEFAULT_TEMPLATE_DIR, help="Path to html_template directory")
    p.add_argument("--template-name", default=DEFAULT_TEMPLATE_NAME, help="Template name")
    p.add_argument("--out-dir", default=DEFAULT_OUT_DIR, help="Output directory for generated HTML")

    p.add_argument("--keep-output", action="store_true", help="Do not delete output dir before running")
    p.add_argument("--verbose-cli", action="store_true", help="Print CLI stdout/stderr even on success")
    return p.parse_args()


def _find_latest_html(out_dir: str) -> str:
    htmls = [
        os.path.join(out_dir, f)
        for f in os.listdir(out_dir)
        if f.endswith(".html") and os.path.isfile(os.path.join(out_dir, f))
    ]
    if not htmls:
        raise FileNotFoundError(f"No HTML generated in {out_dir}")
    return max(htmls, key=os.path.getmtime)


def _main(args) -> NoReturn:
    tqfs_path = os.path.abspath(args.tqfs_path)
    out_dir = os.path.abspath(args.out_dir)

    allfiles = [
        f for f in os.listdir(tqfs_path)
        if os.path.isfile(os.path.join(tqfs_path, f)) and f.endswith(".tqf")
    ]

    with open(os.path.join(out_dir, LOGFILE_NAME), "wt", encoding="utf-8") as f:
        f.write("Execution started\n")

    for fname in allfiles:
        _run_one(args, fname)

    with open(os.path.join(out_dir, LOGFILE_NAME), "at", encoding="utf-8") as f:
        f.write("Execution complete!\n")


def _run_one(args, filename: str):
    out_dir = os.path.abspath(args.out_dir)
    tqfs_path = os.path.abspath(args.tqfs_path)

    command = [
        os.path.abspath(args.cdss_cli),
        "-d", os.path.abspath(args.drug_files_dir),
        "-l", os.path.abspath(args.language_files_dir),
        "-i", os.path.join(tqfs_path, filename),
        "-c", os.path.abspath(args.config_fpath),
        "-t", args.template_name,
        "-p", os.path.abspath(args.template_dir),
        "-o", out_dir,
        "-j",
    ]

    logging.info("Command to execute: %s", " ".join(command))

    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()

    # Show outputs: stderr isn't always a fatal error.
    if args.verbose_cli or process.returncode != 0:
        if stdout:
            logging.info("--- stdout ---\n%s", stdout.decode(errors="replace"))
        if stderr:
            level = logging.ERROR if process.returncode != 0 else logging.WARNING
            logging.log(level, "--- stderr ---\n%s", stderr.decode(errors="replace"))

    logging.info("Process returned code: %d", process.returncode)

    if process.returncode in (0, 2):
        generated = _find_latest_html(out_dir)
        target = os.path.join(out_dir, filename.replace(".tqf", ".html"))
        os.replace(generated, target)
    else:
        with open(os.path.join(out_dir, LOGFILE_NAME), "at", encoding="utf-8") as f:
            f.write(f"EXECUTION FAILED for test [file: {os.path.join(tqfs_path, filename)}]\n")


if __name__ == "__main__":
    args = parse_args()

    if not os.path.isfile(args.config_fpath):
        raise FileNotFoundError(f"Invalid config file: {args.config_fpath}")
    if not os.path.isdir(args.tqfs_path):
        raise FileNotFoundError(f"Invalid TQF directory: {args.tqfs_path}")
    if not os.path.isfile(args.cdss_cli):
        raise FileNotFoundError(f"Missing CDSS CLI binary: {args.cdss_cli}")

    if (not args.keep_output) and os.path.isdir(args.out_dir):
        shutil.rmtree(args.out_dir)
    os.makedirs(args.out_dir, exist_ok=True)

    _main(args)
