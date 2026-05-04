# SPDX-License-Identifier: AGPL-3.0-or-later
project = 'Tucuxi-CDSS'
copyright = '2026, Tucuxi dev team'
author = 'Tucuxi dev team'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

import os
import sys
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '_ext')))

extensions = [
    'sphinx.ext.todo',
    'sphinx.ext.mathjax',
    'sphinx.ext.ifconfig',
    'sphinxcontrib.plantuml',
    'sphinxcontrib.bibtex',
    "glossary_abbr_postprocess"
]

plantuml_output_format = 'svg'

templates_path = ['_templates']
exclude_patterns = []

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# If true, `todo` and `todoList` produce output, else they produce nothing.
todo_include_todos = True

bibtex_bibfiles = ['references.bib']
bibtex_default_style = 'alpha'

rst_prolog = """
.. |AUC| replace:: :abbrev:`AUC`
.. |CDSS| replace:: :abbrev:`CDSS`
.. |CI| replace:: :abbrev:`CI`
.. |CLI| replace:: :abbrev:`CLI`
.. |Cmax| replace:: :abbrev:`Cmax`
.. |Cmin| replace:: :abbrev:`Cmin`
.. |DOM| replace:: :abbrev:`DOM`
.. |IEC62304| replace:: :abbrev:`IEC 62304`
.. |IIV| replace:: :abbrev:`IIV`
.. |JSON| replace:: :abbrev:`JSON`
.. |MAP| replace:: :abbrev:`MAP`
.. |MIPD| replace:: :abbrev:`MIPD`
.. |PD| replace:: :abbrev:`PD`
.. |PK| replace:: :abbrev:`PK`
.. |PKPD| replace:: :abbrev:`PK/PD`
.. |POC| replace:: :abbrev:`POC`
.. |popPK| replace:: :abbrev:`popPK`
.. |SDS| replace:: :abbrev:`SDS`
.. |SI| replace:: :abbrev:`SI`
.. |SRS| replace:: :abbrev:`SRS`
.. |TB| replace:: :abbrev:`TB`
.. |TC| replace:: :abbrev:`TC`
.. |TDM| replace:: :abbrev:`TDM`
.. |TQF| replace:: :abbrev:`TQF`
.. |TS| replace:: :abbrev:`TS`
.. |VaV| replace:: :abbrev:`V&V`
.. |VVP| replace:: :abbrev:`VVP`
.. |VVR| replace:: :abbrev:`VVR`
.. |XML| replace:: :abbrev:`XML`
"""

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'alabaster'
html_static_path = ['_static']

def setup(app):
    app.add_css_file('style.css')

# -- Options for HTML output ---------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
##----pour read the doc theme----
import sphinx_rtd_theme
html_theme = "sphinx_rtd_theme"
html_logo = 'images/tucuxi_logo_html.png'
latex_logo = 'images/tucuxi_logo_latex.png'

# -- Options for Latex output --------------------------------------------------
# Ideas taken here: https://www.sphinx-doc.org/en/master/latex.html

latex_elements =  {
'sphinxsetup' : 'VerbatimColor={rgb}{.9,.9,.9},VerbatimBorderColor={rgb}{1,1,1}',
    'preamble': r'''
\usepackage{enumitem}
\setlistdepth{9}
\setlist[itemize,1]{label=$\bullet$}
\setlist[itemize,2]{label=$\bullet$}
\setlist[itemize,3]{label=$\bullet$}
\setlist[itemize,4]{label=$\bullet$}
\setlist[itemize,5]{label=$\bullet$}
\setlist[itemize,6]{label=$\bullet$}
\setlist[itemize,7]{label=$\bullet$}
\setlist[itemize,8]{label=$\bullet$}
\setlist[itemize,9]{label=$\bullet$}

\renewlist{itemize}{itemize}{9}

\usepackage{colortbl}

\definecolor{tableheadcolor}{RGB}{240,240,240}
\protected\def\sphinxstyletheadfamily {\cellcolor{tableheadcolor}\sffamily}
\usepackage[titles]{tocloft}
\renewcommand{\arraystretch}{1.3}
\cftsetpnumwidth {1.25cm}\cftsetrmarg{1.5cm}
\setlength{\cftchapnumwidth}{0.75cm}
\setlength{\cftsecindent}{\cftchapnumwidth}
\setlength{\cftsecnumwidth}{1.25cm}
''',
    'fncychap': r'\usepackage[Bjornstrup]{fncychap}',
    'printindex': r'\footnotesize\raggedright\printindex',
}


from docutils import nodes, utils
from docutils.parsers.rst import roles

def req_reference_role(role, rawtext, text, lineno, inliner,
                       options={}, content=[]):
    try:
        rfcnum = int(text)
        if rfcnum < 0:
            raise ValueError
    except ValueError:
        msg = inliner.reporter.error(
            'req number must be a number greater than or equal to 0; '
            '"%s" is invalid.' % text, line=lineno)
        prb = inliner.problematic(rawtext, rawtext, msg)
        return [prb], [msg]
    # Base URL mainly used by inliner.rfc_reference, so this is correct:
    ref = inliner.document.settings.rfc_base_url + inliner.rfc_url % rfcnum
    node = nodes.literal(rawtext, 'Requirement ' + utils.unescape(text))
    return [node], []


roles.register_local_role('req', req_reference_role)
