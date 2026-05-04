from docutils import nodes
from docutils.parsers.rst import roles
from sphinx.application import Sphinx

# ----- Step 1: Placeholder role -----
def abbrev_role(name, rawtext, text, lineno, inliner, options={}, content=[]):
    """
    This role just marks text as a placeholder for post-processing.
    """
    node = nodes.inline(text, text)
    node['classes'].append('abbrev')
    return [node], []

# Register the role
roles.register_local_role('abbrev', abbrev_role)

# ----- Step 2: Build glossary mapping after env is ready -----
def build_abbrev_mapping(app: Sphinx, env):
    """
    Build a mapping: term -> (definition, docname, anchor)
    """
    mapping = {}
    for docname in env.found_docs:
        doctree = env.get_doctree(docname)
        for item in doctree.findall(nodes.definition_list_item):
            term_node = item.next_node(nodes.term)
            def_node = item.next_node(nodes.definition)
            if term_node and def_node:
                term = term_node.astext().strip()
                definition = def_node.astext().strip()
                anchor_ids = term_node.get("ids", [])
                if anchor_ids:
                    mapping[term] = (definition, docname, anchor_ids[0])
    env.abbrev_mapping = mapping

# ----- Step 3: Replace placeholders with <abbr> + link -----
def replace_placeholders(app: Sphinx, doctree, docname):
    """
    Replace nodes with class 'abbrev' by HTML <abbr> + link.
    """
    mapping = getattr(app.env, "abbrev_mapping", {})
    builder = app.builder

    for node in doctree.findall(nodes.inline):
        if 'abbrev' in node.get('classes', []):
            term = node.astext().strip()
            entry = mapping.get(term)
            if entry:
                definition, glossary_doc, anchor = entry
                safe_def = definition.replace('"', "&quot;")
                uri = builder.get_relative_uri(docname, glossary_doc) + f"#{anchor}"
                html = f'<a href="{uri}"><abbr title="{safe_def}">{term}</abbr></a>'
                node.replace_self(nodes.raw("", html, format="html"))
            else:
                # fallback: just <abbr> with empty title
                node.replace_self(nodes.raw("", f'<abbr title="">{term}</abbr>', format="html"))

# ----- Step 4: Setup extension -----
def setup(app: Sphinx):
    app.connect("env-updated", build_abbrev_mapping)
    app.connect("doctree-resolved", replace_placeholders)
    return {
        "version": "1.0",
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
