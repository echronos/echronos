from prj import Module


class SimpleSemaphoreTestModule(Module):
    xml_schema = """
  <schema>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="semaphores" type="list" auto_index_field="idx">
     <entry name="semaphore" type="dict">
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-simple-semaphore-test.h', 'render': True},
        {'input': 'rtos-simple-semaphore-test.c', 'render': True, 'type': 'c'},
    ]

module = SimpleSemaphoreTestModule()
