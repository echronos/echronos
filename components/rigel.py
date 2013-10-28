from prj import SystemParseError, Module


class RigelModule(Module):
    xml_schema = """
  <schema>
   <entry name="signalset_size" type="int" default="8"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="fatal_error" type="c_ident" />
   <entry name="signals" type="list" auto_index_field="idx">
     <entry name="signal" type="dict">
       <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="entry" type="c_ident" />
      <entry name="name" type="ident" />
      <entry name="start" type="bool" default="false" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="irq_events" type="list" auto_index_field="idx">
     <entry name="irq_event" type="dict">
      <entry name="name" type="ident" />
      <entry name="task" type="object" group="tasks" />
      <entry name="sig_set" type="int" />
     </entry>
   </entry>
   <entry name="mutexes" type="list" auto_index_field="idx">
     <entry name="mutex" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="timers" type="list" auto_index_field="idx">
     <entry name="timer" type="dict">
      <entry name="name" type="ident" />
      <entry name="enabled" type="bool" />
      <entry name="reload" type="int" />
      <entry name="error" type="int" default="0" />
      <entry name="task" type="object" group="tasks" optional="true" />
      <entry name="sig_set" type="int" optional="true" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-rigel.h', 'render': True},
        {'input': 'rtos-rigel.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        # Ensure that at least one task is runnable.
        if not any(task['start'] for task in config['tasks']):
            raise SystemParseError("At least one task must be configured to start.")

        # Semi-configurable items
        # These are configurable in the code, but for simplicitly they are not supported as
        # user configuration at this stage.
        config['irqeventid_size'] = 8
        config['taskid_size'] = 8

        # Create builtin signals
        timer_signal = {'name': '_task_timer', 'idx': len(config['signals'])}
        config['signals'].append(timer_signal)

        # The RTOS utility signal is used in the following conditions:
        #   1. To start the task.
        #   2. To notify the task when a mutex is unlocked.
        #   3. To notify the task when a message queue has available messages / space
        #
        # The same signal is re-used to avoid excessive allocation of signals.
        # This is safe as a task can not be simultanesouly waiting to start,
        # waiting for a mutex, and waiting on a message queue.
        start_signal = {'name': '_rtos_util', 'idx': len(config['signals'])}
        config['signals'].append(start_signal)

        # Create signal_set definitions from signal definitions:
        config['signal_sets'] = [{'name': sig['name'], 'value': 1 << sig['idx'], 'singleton': True}
                                 for sig in config['signals']]

        # Create a timer for each task
        for task in config['tasks']:
            timer = {'name': '_task_' + task['name'],
                     'error': 0,
                     'reload': 0,
                     'task': task,
                     'idx': len(config['timers']),
                     'enabled': False,
                     'sig_set': (1 << timer_signal['idx'])}
            task['timer'] = timer
            config['timers'].append(timer)
        return config

module = RigelModule()
