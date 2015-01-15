from prj import Module


class TimerTestModule(Module):
    xml_schema = """
<schema>
    <entry name="variant" type="c_ident" />
</schema>"""

    files = [
        {'input': 'timer-test.c', 'render': True, 'type': 'c'},
    ]

module = TimerTestModule()
