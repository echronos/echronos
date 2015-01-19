from prj import Module


class AcamarConfigDemoModule(Module):
    xml_schema = """<schema></schema>"""

    files = [
        {'input': 'acamar-config-demo.c', 'render': True, 'type': 'c'},
    ]

module = AcamarConfigDemoModule()
