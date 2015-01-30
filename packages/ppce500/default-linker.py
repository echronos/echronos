# @LICENSE(NICTA)

from prj import Module


class DefaultLinkerModule(Module):
    # Default stack size value is arbitrary.
    xml_schema = """
<schema>
    <entry name="stack_size" type="int" default="0x1000" />
</schema>"""

    files = [
        {'input': 'default.ld', 'render': True, 'type': 'linker_script', 'stage': 'post_prepare'},
    ]

module = DefaultLinkerModule()
