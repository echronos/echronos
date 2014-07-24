from prj import Module
import logging
logger = logging.getLogger()


class EntryModule(Module):
    # Default stack size value is arbitrary.
    # For interrupt handlers an empty string (the default) will simply generate a vector that loops forever on itself.
    # Alternatively, setting the handler to "undefined" will generate a handler that first creates a stack frame for
    # the interrupted context and stores its registers there before looping forever at location "undefined".
    # Apart from the few handlers whose handler-wrappers (defined in vectable.s) clear their relevant MSR bits, the
    # given handler must be responsible for clearing the condition that caused its interrupt.
    xml_schema = """
<schema>
    <entry name="stack_size" type="int" default="0x1000" />

    <entry name="critical_input" type="c_ident" default="" />
    <entry name="machine_check" type="c_ident" default="" />
    <entry name="data_storage" type="c_ident" default="" />
    <entry name="instruction_storage" type="c_ident" default="" />
    <entry name="external_input" type="c_ident" default="" />
    <entry name="alignment" type="c_ident" default="" />
    <entry name="program" type="c_ident" default="" />
    <entry name="floating_point" type="c_ident" default="" />
    <entry name="system_call" type="c_ident" default="" />
    <entry name="aux_processor" type="c_ident" default="" />
    <entry name="decrementer" type="c_ident" default="" />
    <entry name="fixed_interval_timer" type="c_ident" default="" />
    <entry name="watchdog_timer" type="c_ident" default="" />
    <entry name="data_tlb_error" type="c_ident" default="" />
    <entry name="instruction_tlb_error" type="c_ident" default="" />
    <entry name="debug" type="c_ident" default="" />
    <entry name="eis_spe_apu" type="c_ident" default="" />
    <entry name="eis_fp_data" type="c_ident" default="" />
    <entry name="eis_fp_round" type="c_ident" default="" />
    <entry name="eis_perf_monitor" type="c_ident" default="" />

</schema>"""

    files = [
        {'input': 'vectable.s', 'render': True, 'type': 'asm'},
        {'input': 'default.ld', 'render': True, 'type': 'linker_script', 'stage': 'post_prepare'},
    ]

module = EntryModule()
