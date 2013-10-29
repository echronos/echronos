from prj import Module


class AcamarModule(Module):
    xml_schema = """
<schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="prefix" type="ident" optional="true" />
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="function" type="c_ident" />
      <entry name="name" type="ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
</schema>
"""
    files = [
        {'input': 'rtos-acamar.h', 'render': True},
        {'input': 'rtos-acamar.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        config['prefix_func'] = config['prefix'] + '_' if config['prefix'] is not None else ''
        config['prefix_type'] = config['prefix'].capitalize() if config['prefix'] is not None else ''

        return config

module = AcamarModule()
