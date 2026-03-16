# SPDX-License-Identifier: AGPL-3.0-or-later
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Generate all possible boundary limit violations for a given treatment, then
invoke the CDSS CLI on each corresponding vignette to generate the HTML report.
Missing and empty fields are also artificially inserted to further stress the
code.
In the report, the title is altered to reflect the change that has been
performed in the vignette --- so that the expected effects on the report can be
easily obtained.
"""

__author__ = "Roberto Rigamonti"
__copyright__ = "Copyright 2024, REDS institute --- HEIG-VD"
__maintainer__ = "Roberto Rigamonti"
__contact__ = "roberto.rigamonti@heig-vd.ch"

from bs4 import BeautifulSoup

import copy
from datetime import datetime, timedelta
import itertools
import logging
import os
from random import randrange
import shutil
import subprocess
import sys
import xml.dom.minidom
import argparse

from typing import NoReturn

DEFAULT_TQF_DIR_PATH = "tqf"
DEFAULT_OUT_DIR_PATH = "out"
LOGFILE_NAME = "000_LOG.txt"

DEFAULT_CDSS_CLI_PATH = "../../build/tucuxi_cdss_cli"
DEFAULT_DRUG_FILES_DIR = "../../../tucuxi-drugs/drugfiles/"
DEFAULT_LANGUAGE_FILES_DIR = "../../language/"
DEFAULT_HTML_TEMPLATES_DIR = "../../html_template/"
DEFAULT_TEMPLATE_NAME = "chuv_imatinib_full"

# Flag used to select whether only the typical range has to be validated or not.
CHECK_TYPICAL_RANGE_ONLY = False

# Flag used to perform the cartesian product of the possible combinations --- use
# with care, it will explode the computation time!
PERFORM_CARTESIAN_PRODUCT = False

# Logger configuration.
logging.basicConfig(level=logging.DEBUG)
logger_blocklist = [
    "watchdog",
    "matplotlib",
    "PIL",
]
for module in logger_blocklist:
    logging.getLogger(module).setLevel(logging.WARNING)

def parse_args():
    p = argparse.ArgumentParser(
        description="Generate boundary-limit violations and run CDSS CLI to produce HTML reports."
    )
    p.add_argument("config_fpath", help="Path to config.xml")
    p.add_argument("template_tqf_fpath", help="Path to template .tqf")

    p.add_argument("--cdss-cli", default=DEFAULT_CDSS_CLI_PATH, help="Path to tucuxi_cdss_cli")
    p.add_argument("--drug-files-dir", default=DEFAULT_DRUG_FILES_DIR, help="Drug files directory")
    p.add_argument("--language-files-dir", default=DEFAULT_LANGUAGE_FILES_DIR, help="Language directory")
    p.add_argument("--html-templates-dir", default=DEFAULT_HTML_TEMPLATES_DIR, help="HTML templates directory")
    p.add_argument("--template-name", default=DEFAULT_TEMPLATE_NAME, help="Template name (used by -t)")

    p.add_argument("--tqf-dir", default=DEFAULT_TQF_DIR_PATH, help="Directory to generate TQF files into")
    p.add_argument("--out-dir", default=DEFAULT_OUT_DIR_PATH, help="Directory to generate HTML reports into")

    p.add_argument("--keep-output", action="store_true", help="Do not delete tqf/out directories before running")
    return p.parse_args()

def _main(args) -> NoReturn:
    """
    Main processing function. Loads the configuration file and the template,
    then combinatorially generate the all the missing/valid/invalid covariate
    values, one combination per file.
    For each file, the query date is changed as this gives the name to the
    output HTML file.
    The CDSS CLI is then invoked on the generated TQF, and the title of the
    generated report is altered to reflect the changes made to the covariates.

    Parameters
    ----------
    config_fpath: path of the configuration file
    tmpl_tqf_fpath: path of the template TQF file (with an empty covariates
                    section)
    """
    config_fpath = os.path.abspath(args.config_fpath)
    tmpl_tqf_fpath = os.path.abspath(args.template_tqf_fpath)

    tqf_dir = os.path.abspath(args.tqf_dir)
    out_dir = os.path.abspath(args.out_dir)

    cdss_cli = os.path.abspath(args.cdss_cli)
    drug_dir = os.path.abspath(args.drug_files_dir)
    lang_dir = os.path.abspath(args.language_files_dir)
    templates_dir = os.path.abspath(args.html_templates_dir)
    # Get the configId of the treatment from the template TQF.
    with open(tmpl_tqf_fpath, "rt", encoding="utf-8") as f:
        tmpl_tqf_xml = BeautifulSoup(f, "xml")
        try:
            tqf_configId = (
                tmpl_tqf_xml.query.requests.xpertRequest.configId.string
            )
        except Exception:
            logging.error("Missing treatment configId in the template TQF")
            sys.exit(-2)

    with open(os.path.join(out_dir, LOGFILE_NAME), "wt") as f:
        f.write("Execution started\n")

    # Seek the covariates (and their boundaries) in the configuration file.
    cov_values = {}
    with open(config_fpath, "rt", encoding="utf-8") as f:
        config_xml = BeautifulSoup(f, "xml")
        found = False
        for config in config_xml.cdssConfig.configs.find_all("config"):
            if config.configId.string == tqf_configId:
                for covariate in config.covariates.find_all("covariate"):
                    cov_id = covariate.covariateId.string
                    if cov_id in ["age", "ageInDays", "ageInMonths"]:
                        continue

                    cov_values[cov_id] = []
                    lower_bound = None
                    upper_bound = None

                    if covariate.dataType.string == "bool":
                        descr = cov_id + " invalid bool (neg)"
                        cov_values[cov_id].append(
                            {
                                "field": cov_id,
                                "value": -1,
                                "description": descr,
                            }
                        )
                        descr = cov_id + " invalid bool (pos)"
                        cov_values[cov_id].append(
                            {
                                "field": cov_id,
                                "value": 11,
                                "description": descr,
                            }
                        )
                        descr = cov_id + " valid bool (false)"
                        cov_values[cov_id].append(
                            {
                                "field": cov_id,
                                "value": 0,
                                "description": descr,
                            }
                        )
                        descr = cov_id + " valid bool (true)"
                        cov_values[cov_id].append(
                            {
                                "field": cov_id,
                                "value": 1,
                                "description": descr,
                            }
                        )
                        continue


                    if not CHECK_TYPICAL_RANGE_ONLY:
                        for constraint in covariate.validRange.find_all(
                            "constraint"
                        ):
                            op = constraint.operator.string
                            value = float(constraint.value.string)
                            if op in ["geq", "gt"]:
                                lower_bound = value

                                descr = cov_id + " = LB [" + op + "]"
                                cov_values[cov_id].append(
                                    {
                                        "field": cov_id,
                                        "value": value,
                                        "description": descr,
                                    }
                                )
                                descr = cov_id + " < LB"
                                if value != 0:
                                    cov_values[cov_id].append(
                                        {
                                            "field": cov_id,
                                            "value": round(value * 0.5, 2),
                                            "description": descr,
                                        }
                                    )
                                else:
                                    cov_values[cov_id].append(
                                        {
                                            "field": cov_id,
                                            "value": -1.0,
                                            "description": descr,
                                        }
                                    )
                                if value > 0:
                                    descr = cov_id + " = 0"
                                    cov_values[cov_id].append(
                                        {
                                            "field": cov_id,
                                            "value": 0.0,
                                            "description": descr,
                                        }
                                    )
                                    descr = cov_id + " < 0"
                                    cov_values[cov_id].append(
                                        {
                                            "field": cov_id,
                                            "value": -value,
                                            "description": descr,
                                        }
                                    )
                            elif op in ["leq", "lt"]:
                                upper_bound = value

                                descr = cov_id + " = UB [" + op + "]"
                                cov_values[cov_id].append(
                                    {
                                        "field": cov_id,
                                        "value": value,
                                        "description": descr,
                                    }
                                )
                                descr = cov_id + " > UB"
                                if value != 0:
                                    cov_values[cov_id].append(
                                        {
                                            "field": cov_id,
                                            "value": round(value * 1.5, 2),
                                            "description": descr,
                                        }
                                    )
                                else:
                                    cov_values[cov_id].append(
                                        {
                                            "field": cov_id,
                                            "value": 1.0,
                                            "description": descr,
                                        }
                                    )
                                if value < 0:
                                    descr = cov_id + " = 0"
                                    cov_values[cov_id].append(
                                        {
                                            "field": cov_id,
                                            "value": 0.0,
                                            "description": descr,
                                        }
                                    )
                                    descr = cov_id + " > 0"
                                    cov_values[cov_id].append(
                                        {
                                            "field": cov_id,
                                            "value": -value,
                                            "description": descr,
                                        }
                                    )
                            else:
                                logging.error(
                                    "Unsupported valid range operator: "
                                    + constraint.operator.string
                                )
                                sys.exit(-3)

                    for constraint in covariate.typicalRange.find_all(
                        "constraint"
                    ):
                        op = constraint.operator.string
                        value = float(constraint.value.string)
                        if op in ["geq", "gt"]:
                            descr = cov_id + " = typical LB [" + op + "]"
                            cov_values[cov_id].append(
                                {
                                    "field": cov_id,
                                    "value": value,
                                    "description": descr,
                                }
                            )
                            if lower_bound is not None:
                                descr = (
                                    cov_id
                                    + " between typical and "
                                    + "absolute LB"
                                )
                                cov_values[cov_id].append(
                                    {
                                        "field": cov_id,
                                        "value": round(
                                            (lower_bound + value) / 2, 2
                                        ),
                                        "description": descr,
                                    }
                                )
                            else:
                                descr = cov_id + " < typical LB"
                                tmp_val = value * 0.5
                                if value == 0:
                                    tmp_val = -value
                                cov_values[cov_id].append(
                                    {
                                        "field": cov_id,
                                        "value": round(tmp_val, 2),
                                        "description": descr,
                                    }
                                )
                        elif op in ["leq", "lt"]:
                            descr = cov_id + " = typical UB [" + op + "]"
                            cov_values[cov_id].append(
                                {
                                    "field": cov_id,
                                    "value": value,
                                    "description": descr,
                                }
                            )
                            if upper_bound is not None:
                                descr = (
                                    cov_id
                                    + " between typical and "
                                    + "absolute UB"
                                )
                                cov_values[cov_id].append(
                                    {
                                        "field": cov_id,
                                        "value": round(
                                            (upper_bound + value) / 2, 2
                                        ),
                                        "description": descr,
                                    }
                                )
                            else:
                                descr = cov_id + " > typical UB"
                                cov_values[cov_id].append(
                                    {
                                        "field": cov_id,
                                        "value": round(value * 1.5, 2),
                                        "description": descr,
                                    }
                                )
                        else:
                            logging.error(
                                "Unsupported typical range operator: "
                                + constraint.operator.string
                            )
                            sys.exit(-4)

                found = True
                break
        if not found:
            logging.error("Missing configId in the configuration file")
            sys.exit(-5)
    if len(cov_values) == 0:
        logging.error("Empty covariate values list !")
        sys.exit(-6)
    # Add missing covariance and empty values
    for cov_id in cov_values:
        cov_values[cov_id].append(
            {"field": cov_id, "description": cov_id + " missing"}
        )
        cov_values[cov_id].append(
            {"field": cov_id, "value": "", "description": cov_id + " empty"}
        )
    value_list = []
    for _, values in cov_values.items():
        value_list.append(values)

    # The combinations to explore are the cartesian product of the possible
    # values for all covariates.
    if PERFORM_CARTESIAN_PRODUCT:
        test_list = list(itertools.product(*value_list))
    else:
        # We generate random examples that encompass all the required covariates,
        # paying particular attention to the missing and empty fields (the other
        # combinations are random, but we have to be sure that those cases are
        # dealt with separately, since they might make the program produce no
        # output, and thus the other variables might not be covered as we
        # expected).
        values_no_empty_missing = []
        values_empty_missing = []
        for t_vals in value_list:
            values_no_empty_missing.append(
                [x for x in t_vals if "value" in x and x["value"] != ""]
            )
            values_empty_missing.append(
                [x for x in t_vals if "value" not in x or x["value"] == ""]
            )
        # Start with 'valid' values.
        used_vals = []
        for t_vals in values_no_empty_missing:
            used_vals.append([False] * len(t_vals))
        test_list = []
        while not all(x for y in used_vals for x in y):
            sample = []
            for t_vals, used in zip(values_no_empty_missing, used_vals):
                idx = randrange(len(t_vals))
                used[idx] = True
                sample.append(t_vals[idx])
            test_list.append(sample)
        # Add empty/missing values.
        for bad_cov_idx in range(len(values_empty_missing)):
            for bad_val_idx in range(len(values_empty_missing[bad_cov_idx])):
                sample = [values_empty_missing[bad_cov_idx][bad_val_idx]]
                for cov_idx in range(len(values_no_empty_missing)):
                    if cov_idx != bad_cov_idx:
                        idx = randrange(len(values_no_empty_missing[cov_idx]))
                        sample.append(values_no_empty_missing[cov_idx][idx])
                test_list.append(sample)
    # Generate the set of TQF files.
    with open(tmpl_tqf_fpath, "rt", encoding="utf-8") as f:
        tmpl_tqf_xml = BeautifulSoup(f, "xml", preserve_whitespace_tags=["p"])
        drug_id = tmpl_tqf_xml.query.requests.xpertRequest.drugId.string
        date_str = tmpl_tqf_xml.query.date.string.split("T")[0]
        current_date = datetime.strptime(date_str, "%Y-%m-%d").date()
        cnt = 0
        for test in test_list:
            cnt += 1
            logging.info("Running test " + str(cnt) + "/" + str(len(test_list)))
            new_xml = copy.deepcopy(tmpl_tqf_xml)
            current_date += timedelta(days=1)
            current_date_str = current_date.strftime("%Y-%m-%dT00:00:00")
            new_xml.query.date.string = current_date_str
            covariates = new_xml.drugTreatment.patient.covariates
            for covariate in covariates.find_all("covariate"):
                for test_val in test:
                    if test_val["field"] == covariate.covariateId.string:
                        if "value" in test_val:
                            covariate.value.string = str(test_val["value"])
                        else:
                            covariate.decompose()
                        break

            out_tqf_fname = "test_" + current_date.strftime("%Y_%m_%d") + ".tqf"

            xml_parsed = xml.dom.minidom.parseString(str(new_xml))
            outputstring = xml_parsed.toprettyxml()
            dom_string = os.linesep.join(
                [s for s in outputstring.splitlines() if s.strip()]
            )

            with open(
                os.path.join(tqf_dir, out_tqf_fname),
                "wt",
                encoding="utf-8",
            ) as out_f:
                out_f.write(str(dom_string))

            cur_path = str(os.path.abspath(os.getcwd()))
            command = [
                cdss_cli,
                "-d", drug_dir,
                "-l", lang_dir,
                "-i", os.path.join(tqf_dir, out_tqf_fname),
                "-c", config_fpath,
                "-p", templates_dir,
                "-t", args.template_name,
                "-o", out_dir,
            ]
            process = subprocess.Popen(
                command, stdout=subprocess.PIPE, stderr=subprocess.PIPE
            )

            logging.info("Command to execute: " + " ".join(command))

            stdout, stderr = process.communicate()
            logging.info("--- tucuxi-cdss-core output ---")
            if len(stderr) > 0:
                logging.error("--- tucuxi-cdss-core errors ---")
                logging.error(stderr.decode())

            if process.returncode == 0:
                out_html_fname = (
                    drug_id
                    + "_1_"
                    + current_date.strftime("%-d-%-m-%Y_%-Hh%-Mm%-Ss")
                    + ".html"
                )
                out_fpath = os.path.join(out_dir, out_html_fname)
                if not os.path.isfile(out_fpath):
                    logging.error("Missing output file " + str(out_fpath))
                    sys.exit(-7)

                with open(out_fpath, "rt", encoding="utf-8") as out_f:
                    lines = out_f.read().splitlines()

                new_title = ""
                for test_val in test[:-1]:
                    new_title += test_val["description"]
                    new_title += ", "
                new_title += test[-1]["description"]

                with open(out_fpath, "wt", encoding="utf-8") as out_f:
                    for line in lines:
                        if "<title>Report Layout</title>" in line:
                            new_line = "<title>" + new_title + "</title>"
                        else:
                            new_line = line
                        out_f.write(new_line + "\n")
            else:
                with open(os.path.join(out_dir, LOGFILE_NAME), "at") as f:
                    test_str = ""
                    for test_val in test[:-1]:
                        test_str += test_val["description"]
                        test_str += ", "
                    test_str += test[-1]["description"]
                    f.write(
                        "EXECUTION FAILED for test input: "
                        + test_str
                        + " [file: "
                        + os.path.join(cur_path, tqf_dir, out_tqf_fname)
                        + "]\n"
                    )

    with open(os.path.join(out_dir, LOGFILE_NAME), "at") as f:
        f.write("Execution complete!\n")


if __name__ == "__main__":
    args = parse_args()

    args.config_fpath = os.path.abspath(args.config_fpath)
    args.template_tqf_fpath = os.path.abspath(args.template_tqf_fpath)
    args.cdss_cli = os.path.abspath(args.cdss_cli)
    args.drug_files_dir = os.path.abspath(args.drug_files_dir)
    args.language_files_dir = os.path.abspath(args.language_files_dir)
    args.html_templates_dir = os.path.abspath(args.html_templates_dir)
    args.tqf_dir = os.path.abspath(args.tqf_dir)
    args.out_dir = os.path.abspath(args.out_dir)

    if not os.path.isfile(args.config_fpath):
        raise FileNotFoundError(f"Invalid configuration file path: {args.config_fpath}")
    if not os.path.isfile(args.template_tqf_fpath):
        raise FileNotFoundError(f"Invalid template TQF file path: {args.template_tqf_fpath}")
    if not os.path.isfile(args.cdss_cli):
        raise FileNotFoundError(f"Missing CDSS CLI binary: {args.cdss_cli}")

    # Prepare dirs
    if not args.keep_output:
        if os.path.isdir(args.tqf_dir):
            shutil.rmtree(args.tqf_dir)
        if os.path.isdir(args.out_dir):
            shutil.rmtree(args.out_dir)

    os.makedirs(args.tqf_dir, exist_ok=True)
    os.makedirs(args.out_dir, exist_ok=True)

    _main(args)
