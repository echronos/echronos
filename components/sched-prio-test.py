from prj import Module


class SchedPrioTestModule(Module):
    xml_schema = """
  <schema>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="semaphores" type="list" auto_index_field="idx">
     <entry name="semaphore" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-sched-prio-test.h', 'render': True},
        {'input': 'rtos-sched-prio-test.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        config['prefix_func'] = config['prefix'] + '_' if config['prefix'] is not None else ''

        return config

module = SchedPrioTestModule()
