from prj import Module

class DefaultLinkerModule(Module):

    xml_schema = """
<schema>
    <entry name="load_addr" type="int" default="0" />
    <entry name="prefix" type="c_ident" default="" />
    <entry name="ll_debug" type="c_ident" default="" />
    <entry name="phrase" type="string" default="Hello, world!" />
    <entry name="stack_size" type="int" default="0x1000" />
</schema>"""

    files = [
        {'input': 'default.ld', 'render': True, 'type': 'linker_script', 'stage': 'post_prepare'},
    ]

module = DefaultLinkerModule()
