/*| headers |*/
{{#mpu_enabled}}
{{#mpu_verbose_faults}}
#include "debug.h"
{{/mpu_verbose_faults}}
{{/mpu_enabled}}

/*| object_like_macros |*/
{{#mpu_enabled}}
/* MPU-related constants */
#define MPU_MAX_REGIONS             8
#define MPU_BUILTIN_REGIONS         2           /* Stack & flash regions           */
#define MPU_MAX_ASSOCIATED_DOMAINS  (MPU_MAX_REGIONS-MPU_BUILTIN_REGIONS)

/* MPU control registers */
#define MPU_TYPE                    0xE000ED90  /* MPU Type                        */
#define MPU_CTRL                    0xE000ED94  /* MPU Control                     */
#define MPU_NUMBER                  0xE000ED98  /* MPU Region Number               */
#define MPU_BASE                    0xE000ED9C  /* MPU Region Base Address         */
#define MPU_ATTR                    0xE000EDA0  /* MPU Region Attribute and Size   */
                                                /* We exclude the additional alias
                                                 * regions as they are unavailable
                                                 * on some processors */
/* Interrupt control registers */
#define MPU_SYS_HND_CTRL            0xE000ED24  /* System Handler Control and State */
#define MPU_SYS_HND_CTRL_MEM        0x00010000  /* Memory Management Fault Enable   */

/* Fault status registers */
#define MPU_NVIC_FAULT_STAT         0xE000ED28  /* Configurable Fault Status        */
#define MPU_NVIC_MM_ADDR            0xE000ED34  /* Memory Management Fault Address  */

/* MPU type bitfields */
#define MPU_TYPE_DREGION_M          0x0000FF00  /* Number of D Regions             */
#define MPU_TYPE_SEPARATE           0x00000001  /* Separate or Unified MPU         */
#define MPU_TYPE_DREGION_S          8

/* MPU base bitfields */
#define MPU_BASE_ADDR_M             0xFFFFFFE0  /* Base Address Mask               */
#define MPU_BASE_VALID              0x00000010  /* Region Number Valid             */
#define MPU_BASE_REGION_M           0x00000007  /* Region Number                   */
#define MPU_BASE_ADDR_S             5
#define MPU_BASE_REGION_S           0

/* MPU control flags */
#define MPU_CTRL_ENABLE             0x00000001  /* MPU Enable                      */

/* MPU enable flags */
#define MPU_CONFIG_PRIV_DEFAULT     4
#define MPU_CONFIG_HARDFLT_NMI      2

/* MPU region enable flags */
#define MPU_RGN_ENABLE              1

/* MPU region permission flags */
#define MPU_P_EXEC                  0x00000000
#define MPU_P_NOEXEC                0x10000000
#define MPU_P_NO                    0x01000000  /* RW for priv, no access for user */
#define MPU_P_RO                    0x02000000  /* RW for priv, read-only for user access */
#define MPU_P_RW                    0x03000000  /* Read-Write for privileged and user access */
#define MPU_P_MASK                  0x0F000000

/* MPU memory type flags */
#define MPU_ATTR_TEX_M              0x00380000
#define MPU_ATTR_SHAREABLE          0x00040000
#define MPU_ATTR_CACHEABLE          0x00020000
#define MPU_ATTR_BUFFRABLE          0x00010000

/* MPU region attribute flags */
#define MPU_ATTR_SIZE_M             0x0000003E  /* Region Size Mask */
#define MPU_ATTR_ENABLE             0x00000001  /* Region Enable    */
{{/mpu_enabled}}

/*| types |*/

/*| structures |*/
{{#mpu_enabled}}
struct mpu_region
{
    uint32_t base_flag;
    uint32_t attr_flag;
};

static struct mpu_region mpu_regions[{{tasks.length}}][MPU_MAX_REGIONS-1] =
{
{{#tasks}}
    {
        /* Hardcoded init for MPU_MAX_REGIONS for now
         * Note an entry for 0 is not required as the code region (#0)
         * has constant configuration across context switches */
        {1 | MPU_BASE_VALID, 0},
        {2 | MPU_BASE_VALID, 0},
        {3 | MPU_BASE_VALID, 0},
        {4 | MPU_BASE_VALID, 0},
        {5 | MPU_BASE_VALID, 0},
        {6 | MPU_BASE_VALID, 0},
        {7 | MPU_BASE_VALID, 0},
    },
{{/tasks}}
};
{{/mpu_enabled}}

/*| extern_declarations |*/
{{#mpu_enabled}}
/* These declarations allow us to use symbols that were declared inside the
 * linker script. For flash & sram this is necessary as these parameters
 * are part of the vectable module. For protection domains, this is necessary
 * as the location of protection domains is computed at link-time.
 *
 * Note the *address* of these symbols must be taken to get their value.
 * This should always be through the 'mpu_linker_value' macro defined below.*/
extern const uint32_t linker_flash_start;
extern const uint32_t linker_flash_size;
extern const uint32_t linker_sram_size;
{{#mpu_protection_domains}}
extern const uint32_t linker_domain_{{name}}_start;
extern const uint32_t linker_domain_{{name}}_size;
{{/mpu_protection_domains}}
{{/mpu_enabled}}

/*| function_declarations |*/
{{#mpu_enabled}}
static bool mpu_is_enabled(void);
static void mpu_enable(void);
static void mpu_disable(void);
static uint32_t mpu_hardware_regions_supported(void);
static bool mpu_hardware_is_unified(void);
static void mpu_region_disable(uint32_t mpu_region);
static uint32_t mpu_get_attr_flag(uint32_t mpu_size_and_permission_flags, uint32_t mpu_base_addr);
static uint32_t mpu_get_base_flag(uint32_t mpu_region_index, uint32_t mpu_base_addr);
static void mpu_memmanage_interrupt_enable(void);
static uint32_t mpu_region_size_flag(uint32_t bytes);
static void mpu_populate_regions(void);
static void mpu_handle_fault(void);
static void mpu_initialize(void);
inline void rtos_internal_mpu_configure_for_current_task(void);
{{/mpu_enabled}}

/*| state |*/

/*| function_like_macros |*/
{{#mpu_enabled}}
#define mpu_hardware_register(x) (*((volatile uint32_t *)(x)))
#define mpu_is_pow2(x) ((x) && !((x) & ((x) - 1)))
#define mpu_linker_value(x) ((uint32_t)&(x))
{{/mpu_enabled}}

/*| functions |*/
{{#mpu_enabled}}
static bool
mpu_is_enabled(void)
{
    return (mpu_hardware_register(MPU_CTRL) & MPU_CTRL_ENABLE);
}

static void
mpu_enable(void)
{
    internal_assert(!mpu_is_enabled(), ERROR_ID_MPU_ALREADY_ENABLED );

    /* Turn on the MPU */
    mpu_hardware_register(MPU_CTRL) |= MPU_CTRL_ENABLE;
}

__attribute__((unused))
static void
mpu_disable(void)
{
    internal_assert(mpu_is_enabled(), ERROR_ID_MPU_ALREADY_DISABLED);

    mpu_hardware_register(MPU_CTRL) &= ~MPU_CTRL_ENABLE;
}

/* Gets the number of hardware regions supported by this MPU */
static uint32_t
mpu_hardware_regions_supported(void)
{
    /* Read the DREGION field of the MPU type register and mask off   */
    /* the bits of interest to get the count of regions.              */
    return ((mpu_hardware_register(MPU_TYPE) & MPU_TYPE_DREGION_M) >> MPU_TYPE_DREGION_S);
}

/* Does our hardware have unified I & D regions? */
static bool
mpu_hardware_is_unified(void)
{
    return !(mpu_hardware_register(MPU_TYPE) & MPU_TYPE_SEPARATE);
}

inline static void
mpu_region_disable(const uint32_t mpu_region)
{
    internal_assert(mpu_region < MPU_MAX_REGIONS, ERROR_ID_MPU_INTERNAL_INVALID_REGION_INDEX);

    mpu_hardware_register(MPU_NUMBER) = mpu_region;
    mpu_hardware_register(MPU_ATTR) &= ~MPU_ATTR_ENABLE;
}

static uint32_t
mpu_get_attr_flag(const uint32_t mpu_size_and_permission_flags, const uint32_t mpu_base_addr)
{
    /* Check that the address is size-aligned as required */
    internal_assert(mpu_base_addr == (mpu_base_addr & ~0 <<
                   (((mpu_size_and_permission_flags & MPU_ATTR_SIZE_M) >> 1) + 1)),
                   ERROR_ID_MPU_INTERNAL_MISALIGNED_ADDR);

    /* Set region attributes, with fixed values for:
     * Type Extension Mask = 0
     * Cacheable = 0
     * Shareable = 1
     * Bufferable = 1
     * These options should be fine on most devices, with a small performance
     * penalty on armv7m processors that have a cache. This is rare though. */
    return ((mpu_size_and_permission_flags & ~(MPU_ATTR_TEX_M | MPU_ATTR_CACHEABLE)) |
            MPU_ATTR_SHAREABLE | MPU_ATTR_BUFFRABLE);
}

static uint32_t
mpu_get_base_flag(const uint32_t mpu_region_index, const uint32_t mpu_base_addr)
{
    internal_assert(mpu_region_index < MPU_MAX_REGIONS, ERROR_ID_MPU_INTERNAL_INVALID_REGION_INDEX);

    /* Combination will select the region and set the base address at the same time */
    return mpu_region_index | mpu_base_addr | MPU_BASE_VALID;
}

static void
mpu_memmanage_interrupt_enable(void)
{
    /* Clear the NVIC FSR as it starts off as junk */
    uint32_t fault_stat = mpu_hardware_register(MPU_NVIC_FAULT_STAT);
    mpu_hardware_register(MPU_NVIC_FAULT_STAT) = fault_stat;

    /* Enable the interrupt */
    mpu_hardware_register(MPU_SYS_HND_CTRL) |= MPU_SYS_HND_CTRL_MEM;
}

static uint32_t
mpu_region_size_flag(const uint32_t bytes)
{
    /* armv7m MPU only supports regions of 2^n size, above 32 bytes */
    internal_assert(mpu_is_pow2(bytes), ERROR_ID_MPU_INVALID_REGION_SIZE);
    internal_assert(bytes >= 32, ERROR_ID_MPU_INVALID_REGION_SIZE);

    /* MPU region size flag for 2^x bytes is (x-1)<<1
     * Count trailing zeros to get log2(x), valid as x is a power of 2 */
    return ((__builtin_ctz(bytes) - 1) << 1);
}

static void
mpu_populate_regions(void)
{
    /* Note: region 0 is always the RX-only flash protection region
     * as set up during MPU initialization, so we use index 0 to
     * actually store region 1 */

    /* We assume that mpu_regions has been initialized to zero
     * as it sits in the .bss section. This is important as
     * we use null regions to check whether to disable them
     * altogether when switching tasks. */

{{#tasks}}
    #if {{mpu_associated_domains.length}} > MPU_MAX_ASSOCIATED_DOMAINS
    #error "Too many associated domains for task: {{name}}"
    #endif

    /* Stack region for task: {{name}} */
    mpu_regions[{{idx}}][0].attr_flag =
        mpu_get_attr_flag(mpu_region_size_flag({{stack_size}}*sizeof(uint32_t)) |
                          MPU_P_NOEXEC | MPU_P_RW | MPU_RGN_ENABLE, (uint32_t)&stack_{{idx}});
    mpu_regions[{{idx}}][0].base_flag = mpu_get_base_flag(1, (uint32_t)&stack_{{idx}});

    /* Protection domains for task: {{name}} */
{{#mpu_associated_domains}}
{{#writeable}}{{^readable}}
    #error "Write-only permissions unsupported on armv7m. Domain: {{name}}"
{{/readable}}{{/writeable}}
    mpu_regions[{{idx}}][{{domx}}+1].base_flag =
        mpu_get_base_flag({{domx}}+2, mpu_linker_value(linker_domain_{{name}}_start));
    mpu_regions[{{idx}}][{{domx}}+1].attr_flag =
        mpu_get_attr_flag(
            mpu_region_size_flag(mpu_linker_value(linker_domain_{{name}}_size)) | MPU_RGN_ENABLE |
                {{#readable}}{{^writeable}}MPU_P_RO{{/writeable}}{{/readable}} /* Read-only? */
                {{#writeable}}{{#readable}}MPU_P_RW{{/readable}}{{/writeable}} /* Read-write? */
                {{^executable}}| MPU_P_NOEXEC{{/executable}} /* Executable? (no flag = executable) */
            , mpu_linker_value(linker_domain_{{name}}_start) );
{{/mpu_associated_domains}}
{{/tasks}}
}

static void
mpu_handle_fault(void)
{
    uint32_t fault_status = mpu_hardware_register(MPU_NVIC_FAULT_STAT);

{{#mpu_verbose_faults}}
    uint32_t fault_address = mpu_hardware_register(MPU_NVIC_MM_ADDR);
    debug_print("protection fault: [address=");
    debug_printhex32(fault_address);
    debug_print(", status=");
    debug_printhex32(fault_status);
    debug_print("]\n");
{{/mpu_verbose_faults}}

    /* Clear the fault status register */
    mpu_hardware_register(MPU_NVIC_FAULT_STAT) = fault_status;
}

static void
mpu_initialize(void)
{
    /* Check hardware registers to see if this processor actually has
     * any MPU hardware. We support MPUs with >= 8 regions and a unified
     * memory model. From the ARM TRM for the Cortex-M series, this
     * encompasses the Cortex M0+, M3, M4 & M7 series processors.
     *
     * The M7 is a bit of a special case in that it has a non-unified
     * option, however I have not found any silicon vendors that actually
     * manufacture a chip using a non-unified MPU. */

    internal_assert(mpu_hardware_regions_supported() >= MPU_MAX_REGIONS, ERROR_ID_MPU_NON_STANDARD);
    internal_assert(mpu_hardware_is_unified(), ERROR_ID_MPU_NON_STANDARD);

    /* Make the MPU use a default region in privileged mode, disable it during a hard fault */
    mpu_hardware_register(MPU_CTRL) |= MPU_CONFIG_PRIV_DEFAULT;
    mpu_hardware_register(MPU_CTRL) &= ~(MPU_CONFIG_HARDFLT_NMI);

    /* Initially, we will only give tasks access to:
     * - Their own stack
     * - Any specifically annotated extra memory regions.
     * We make code read-only, however we do not protect tasks
     * from reading/executing code that is not theirs. This
     * model of protecting data but not the code is standard.
     * See SLOTH, or AUTOSAR OS specifications v5.0.0 */

    /* Create a read-only executable region for our FLASH */
    uint32_t flash_size = mpu_linker_value(linker_flash_size);
    mpu_hardware_register(MPU_BASE) = mpu_get_base_flag(0, mpu_linker_value(linker_flash_start));
    mpu_hardware_register(MPU_ATTR) = mpu_get_attr_flag(
            mpu_region_size_flag(flash_size) | MPU_P_EXEC | MPU_P_RO | MPU_RGN_ENABLE,
            mpu_linker_value(linker_flash_start));

    /* fill up our region table for each task */
    mpu_populate_regions();

    /* Enable the memmanage interrupt */
    mpu_memmanage_interrupt_enable();

    /* The MPU itself will only enforce memory protection rules
     * in usermode. We leave it on for the lifetime of our system. */
    mpu_enable();
}

void
rtos_internal_mpu_configure_for_current_task(void)
{
    /* Note: region 0 is always the RX-only flash protection region
     * as set up during MPU initialization, so we do not touch that.
     * To save space, mpu_regions[x][0] corresponds to MPU region 1.*/

    /* We simply enable and set parameters for the regions we are using
     * and then disable all the regions that we aren't */

    {{prefix_type}}TaskId to = rtos_internal_current_task;
    uint32_t region_config_addr = (uint32_t)&mpu_regions[to][0];

    /* Load the 7 task-specific MPU regions we're using, by exploiting
     * the 8 adjacent MPU region alias registers */
    /* NOTE: Assumes the mpu_regions struct has been packed properly */
    /* The compiler wasn't intelligent enough to optimize this */
    asm volatile
    (
            "ldm %0, {r2-r6, r8-r10}\n"
            "stm %1, {r2-r6, r8-r10}\n"
            "adds %0, #32\n"
            "ldm %0, {r2-r6, r8}\n"
            "stm %1, {r2-r6, r8}\n"
            : "+r" (region_config_addr) : "r" (MPU_BASE)
            : "memory", "r2", "r3", "r4", "r5", "r6", "r8", "r9", "r10"
    );
}

{{#mpu_skip_faulting_instructions}}
__attribute__((naked))
void
rtos_internal_memmanage_handler(void)
{
    /* Load the offending PC, and increment it on the exception stack.
     * when we RFE, we will resume execution after the bad instruction.
     * Note that we only add 2 as we are in thumb mode */
    asm volatile
    (
        "mrs r0, msp\n"
        "ldr r1, [r0, #6*4]\n"
        "add r1, r1, #2\n"
        "str r1, [r0, #6*4]\n"
    );

    mpu_handle_fault();

    /* Must load the lr with this special return value to indicate
     * an RFE (popping stacked registers (including PC) and
     * switching to usermode) */
    asm volatile
    (
        "mvn lr, #6\n"
        "bx lr\n"
    );
}
{{/mpu_skip_faulting_instructions}}

{{^mpu_skip_faulting_instructions}}
void
rtos_internal_memmanage_handler(void)
{
    mpu_handle_fault();

    /* Turn off the MPU in case we managed to block ourselves
     * from doing memory accesses in privileged mode */
    mpu_disable();

    /* An MPU policy violation is a fatal error (normally) */
    {{fatal_error}}(ERROR_ID_MPU_VIOLATION);
}
{{/mpu_skip_faulting_instructions}}
{{/mpu_enabled}}

/*| public_functions |*/

/*| public_privileged_functions |*/
