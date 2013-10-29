from prj import Module


class KrazModule(Module):
    xml_schema = """
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="signalset_size" type="int" default="8"/>
   <entry name="prefix" type="ident" optional="true" />
   <entry name="signals" type="list" auto_index_field="idx">
     <entry name="signal" type="dict">
       <entry name="name" type="ident" />
     </entry>
   </entry>
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
        {'input': 'rtos-kraz.h', 'render': True},
        {'input': 'rtos-kraz.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        # Create signal_set definitions from signal definitions:
        config['signal_sets'] = [{'name': sig['name'], 'value': 1 << sig['idx'], 'singleton': True}
                                 for sig in config['signals']]

        config['prefix_func'] = config['prefix'] + '_' if config['prefix'] is not None else ''

        return config

module = KrazModule()
