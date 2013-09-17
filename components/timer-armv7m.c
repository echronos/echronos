/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix}}timer_tick(void);

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static bool timer_check(void);

/*| state |*/
static bool tick_pending;
static bool tick_overflow;

/*| function_like_macros |*/

/*| functions |*/
static bool
timer_check(void)
{
    bool r = false;

    asm volatile("cpsid i");

    if (tick_pending)
    {
        if (tick_overflow)
        {
            {{fatal_error}}(ERROR_ID_TICK_OVERFLOW);
        }
        r = true;
        tick_pending = false;
    }

    asm volatile("cpsie i");

    return r;
}

/*| public_functions |*/
void
{{prefix}}timer_tick(void)
{
    if (tick_pending)
    {
        tick_overflow = true;
    }
    tick_pending = true;
}
