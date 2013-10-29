from prj import Module


class AcamarModule(Module):
    xml_schema = """
<schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
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

module = AcamarModule()
