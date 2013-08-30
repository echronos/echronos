from prj import Module
from operator import itemgetter


class KochabModule(Module):
    xml_schema = """
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="signalset_size" type="int" default="8"/>
   <entry name="irqeventid_size" type="int" default="8"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="tasks" type="list">
     <entry name="task" type="dict">
      <entry name="entry" type="c_ident" />
      <entry name="priority" type="int" />
      <entry name="name" type="ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="irq_events" type="list" auto_index_field="idx">
     <entry name="irq_event" type="dict">
      <entry name="name" type="ident" />
      <entry name="task" type="int" />
      <entry name="sig_set" type="int" />
     </entry>
   </entry>
   <entry name="mutexes" type="list" auto_index_field="idx">
     <entry name="mutex" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="semaphores" type="list" auto_index_field="idx">
     <entry name="semaphore" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="signals" type="list" auto_index_field="idx">
     <entry name="signal" type="dict">
       <entry name="name" type="ident" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-kochab.h', 'render': True},
        {'input': 'rtos-kochab.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        tasks = config['tasks']
        tasks.sort(key=itemgetter('priority'), reverse=True)
        for idx, t in enumerate(tasks):
            t['idx'] = idx
        return config

module = KochabModule()
