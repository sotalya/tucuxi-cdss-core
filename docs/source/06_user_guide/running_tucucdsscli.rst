.. SPDX-License-Identifier: AGPL-3.0-or-later

Running Tucuxi-CDSS CLI
=======================

The command line tool :program:`tucuxi_cdss_cli` allows for the utilization of
all the core functionalities of Tucuxi-CDSS.

The main objective of :program:`tucuxi_cdss_cli` is to take the necessary input
information and generate a comprehensive report (|XML| or HTML) to inform the
user about medication readjustment or otherwise.

.. important::

   The **sole computation** performed by :program:`tucuxi_cdss_cli` is a
   **dosage adjustment**. The system cannot be used to compute standalone
   concentration predictions, percentile curves, or any other pharmacokinetic
   output independently. Concentration predictions and percentile data that
   appear in the response are computed internally as part of the adjustment
   algorithm and are included to provide context for the recommendation.

All the information required by the tool is in |XML| format. The specifics of
this format can be found here: :ref:`Query Format<queryFormat>`

:program:`tucuxi_cdss_cli` has a very simple interface: It can have nine
arguments:

    1. The path where to find the drug model files
    2. The |XML| file containing the query
    3. The |XML| file containing the configuration for the |CDSS| (Optional)
    4. The path to output folder
    5. The path where language translation files are. (Optional, by default language directory of root project)
    6. The desired language for the output report. (Optional, by default use the language defined in query)
    7. The name of the output template you want to use (Should match directory names)
    8. The path of the directory containing template folders. (Optional, by default template directory of root project)
    9. Option to dump the generated json for debugging. (Optional, disabled by default)


.. code::

    Usage:
        ./tucuxi_cdss_cli [OPTION...]

        -d, --drugpath arg      Path of the directory containing drug files
        -i, --input arg         Path of the input request file
        -c, --config arg        Path of the XML configuration file
        -o, --outputpath arg    Path of the directory where the output report
                                will be created
        -l, --languagepath arg  Path of the directory containing translation
                                files
        -g, --language arg      Desired language for output
        -t, --templatename arg  Name of the HTML template
        -p, --templatepath arg  Path of the directory containing HTML template
                                files
        -j, --jsondump          Enable json dump for debug (in output folder)
            --help              Print this help message


.. _return_values:

Return values
-------------

The return value of :program:`tucuxi_cdss_cli` can be any of the following:

.. csv-table::
   :header: "Code", "Meaning"
   :widths: 8, 70

   "0",  "**All requests succeeded**: every request was fully processed and a report was generated."
   "1",  "**Partial success**: at least one request succeeded and at least one failed."
   "2",  "**No requests succeeded**: no request could be fully processed."
   "3",  "**Bad arguments**: a mandatory argument is missing or refers to a non-existent path."
   "4",  "**Import error**: the query file could not be loaded or parsed."
