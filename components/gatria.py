from prj import Module


class GatriaModule(Module):
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
   <entry name="mutexes" type="list" auto_index_field="idx">
     <entry name="mutex" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-gatria.h', 'render': True},
        {'input': 'rtos-gatria.c', 'render': True, 'type': 'c'},
    ]

module = GatriaModule()
