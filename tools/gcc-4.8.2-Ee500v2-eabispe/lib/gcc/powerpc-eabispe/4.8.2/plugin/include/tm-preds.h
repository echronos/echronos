/* Generated automatically by the program 'build/genpreds'
   from the machine description file '../../src_gcc/gcc/config/rs6000/rs6000.md'.  */

#ifndef GCC_TM_PREDS_H
#define GCC_TM_PREDS_H

#ifdef HAVE_MACHINE_MODES
extern int general_operand (rtx, enum machine_mode);
extern int address_operand (rtx, enum machine_mode);
extern int register_operand (rtx, enum machine_mode);
extern int pmode_register_operand (rtx, enum machine_mode);
extern int scratch_operand (rtx, enum machine_mode);
extern int immediate_operand (rtx, enum machine_mode);
extern int const_int_operand (rtx, enum machine_mode);
extern int const_double_operand (rtx, enum machine_mode);
extern int nonimmediate_operand (rtx, enum machine_mode);
extern int nonmemory_operand (rtx, enum machine_mode);
extern int push_operand (rtx, enum machine_mode);
extern int pop_operand (rtx, enum machine_mode);
extern int memory_operand (rtx, enum machine_mode);
extern int indirect_operand (rtx, enum machine_mode);
extern int ordered_comparison_operator (rtx, enum machine_mode);
extern int comparison_operator (rtx, enum machine_mode);
extern int any_operand (rtx, enum machine_mode);
extern int any_parallel_operand (rtx, enum machine_mode);
extern int count_register_operand (rtx, enum machine_mode);
extern int altivec_register_operand (rtx, enum machine_mode);
extern int vsx_register_operand (rtx, enum machine_mode);
extern int vfloat_operand (rtx, enum machine_mode);
extern int vint_operand (rtx, enum machine_mode);
extern int vlogical_operand (rtx, enum machine_mode);
extern int ca_operand (rtx, enum machine_mode);
extern int s5bit_cint_operand (rtx, enum machine_mode);
extern int u5bit_cint_operand (rtx, enum machine_mode);
extern int s8bit_cint_operand (rtx, enum machine_mode);
extern int short_cint_operand (rtx, enum machine_mode);
extern int u_short_cint_operand (rtx, enum machine_mode);
extern int non_short_cint_operand (rtx, enum machine_mode);
extern int exact_log2_cint_operand (rtx, enum machine_mode);
extern int const_0_to_1_operand (rtx, enum machine_mode);
extern int const_2_to_3_operand (rtx, enum machine_mode);
extern int gpc_reg_operand (rtx, enum machine_mode);
extern int cc_reg_operand (rtx, enum machine_mode);
extern int cc_reg_not_cr0_operand (rtx, enum machine_mode);
extern int cc_reg_not_micro_cr0_operand (rtx, enum machine_mode);
extern int reg_or_short_operand (rtx, enum machine_mode);
extern int reg_or_neg_short_operand (rtx, enum machine_mode);
extern int reg_or_aligned_short_operand (rtx, enum machine_mode);
extern int reg_or_u_short_operand (rtx, enum machine_mode);
extern int reg_or_cint_operand (rtx, enum machine_mode);
extern int add_cint_operand (rtx, enum machine_mode);
extern int reg_or_add_cint_operand (rtx, enum machine_mode);
extern int reg_or_sub_cint_operand (rtx, enum machine_mode);
extern int reg_or_logical_cint_operand (rtx, enum machine_mode);
extern int easy_fp_constant (rtx, enum machine_mode);
extern int easy_vector_constant (rtx, enum machine_mode);
extern int easy_vector_constant_add_self (rtx, enum machine_mode);
extern int easy_vector_constant_msb (rtx, enum machine_mode);
extern int zero_constant (rtx, enum machine_mode);
extern int zero_fp_constant (rtx, enum machine_mode);
extern int volatile_mem_operand (rtx, enum machine_mode);
extern int offsettable_mem_operand (rtx, enum machine_mode);
extern int indexed_or_indirect_operand (rtx, enum machine_mode);
extern int altivec_indexed_or_indirect_operand (rtx, enum machine_mode);
extern int indexed_or_indirect_address (rtx, enum machine_mode);
extern int fix_trunc_dest_operand (rtx, enum machine_mode);
extern int add_operand (rtx, enum machine_mode);
extern int non_add_cint_operand (rtx, enum machine_mode);
extern int logical_const_operand (rtx, enum machine_mode);
extern int logical_operand (rtx, enum machine_mode);
extern int non_logical_cint_operand (rtx, enum machine_mode);
extern int mask_operand (rtx, enum machine_mode);
extern int mask_operand_wrap (rtx, enum machine_mode);
extern int mask64_operand (rtx, enum machine_mode);
extern int mask64_2_operand (rtx, enum machine_mode);
extern int and64_2_operand (rtx, enum machine_mode);
extern int and_operand (rtx, enum machine_mode);
extern int scc_eq_operand (rtx, enum machine_mode);
extern int reg_or_mem_operand (rtx, enum machine_mode);
extern int reg_or_none500mem_operand (rtx, enum machine_mode);
extern int zero_reg_mem_operand (rtx, enum machine_mode);
extern int lwa_operand (rtx, enum machine_mode);
extern int symbol_ref_operand (rtx, enum machine_mode);
extern int got_operand (rtx, enum machine_mode);
extern int got_no_const_operand (rtx, enum machine_mode);
extern int rs6000_tls_symbol_ref (rtx, enum machine_mode);
extern int call_operand (rtx, enum machine_mode);
extern int current_file_function_operand (rtx, enum machine_mode);
extern int input_operand (rtx, enum machine_mode);
extern int splat_input_operand (rtx, enum machine_mode);
extern int rs6000_nonimmediate_operand (rtx, enum machine_mode);
extern int boolean_operator (rtx, enum machine_mode);
extern int boolean_or_operator (rtx, enum machine_mode);
extern int equality_operator (rtx, enum machine_mode);
extern int min_max_operator (rtx, enum machine_mode);
extern int branch_comparison_operator (rtx, enum machine_mode);
extern int rs6000_cbranch_operator (rtx, enum machine_mode);
extern int scc_comparison_operator (rtx, enum machine_mode);
extern int scc_rev_comparison_operator (rtx, enum machine_mode);
extern int branch_positive_comparison_operator (rtx, enum machine_mode);
extern int load_multiple_operation (rtx, enum machine_mode);
extern int store_multiple_operation (rtx, enum machine_mode);
extern int save_world_operation (rtx, enum machine_mode);
extern int restore_world_operation (rtx, enum machine_mode);
extern int vrsave_operation (rtx, enum machine_mode);
extern int mfcr_operation (rtx, enum machine_mode);
extern int mtcrf_operation (rtx, enum machine_mode);
extern int lmw_operation (rtx, enum machine_mode);
extern int stmw_operation (rtx, enum machine_mode);
extern int tie_operand (rtx, enum machine_mode);
extern int small_toc_ref (rtx, enum machine_mode);
#endif /* HAVE_MACHINE_MODES */

#define CONSTRAINT_NUM_DEFINED_P 1
enum constraint_num
{
  CONSTRAINT__UNKNOWN = 0,
  CONSTRAINT_f,
  CONSTRAINT_d,
  CONSTRAINT_b,
  CONSTRAINT_h,
  CONSTRAINT_c,
  CONSTRAINT_l,
  CONSTRAINT_v,
  CONSTRAINT_x,
  CONSTRAINT_y,
  CONSTRAINT_z,
  CONSTRAINT_wd,
  CONSTRAINT_wf,
  CONSTRAINT_ws,
  CONSTRAINT_wa,
  CONSTRAINT_wZ,
  CONSTRAINT_I,
  CONSTRAINT_J,
  CONSTRAINT_K,
  CONSTRAINT_L,
  CONSTRAINT_M,
  CONSTRAINT_N,
  CONSTRAINT_O,
  CONSTRAINT_P,
  CONSTRAINT_G,
  CONSTRAINT_H,
  CONSTRAINT_es,
  CONSTRAINT_Q,
  CONSTRAINT_Y,
  CONSTRAINT_Z,
  CONSTRAINT_a,
  CONSTRAINT_R,
  CONSTRAINT_S,
  CONSTRAINT_T,
  CONSTRAINT_U,
  CONSTRAINT_t,
  CONSTRAINT_W,
  CONSTRAINT_j,
  CONSTRAINT__LIMIT
};

extern enum constraint_num lookup_constraint (const char *);
extern bool constraint_satisfied_p (rtx, enum constraint_num);

static inline size_t
insn_constraint_len (char fc, const char *str ATTRIBUTE_UNUSED)
{
  switch (fc)
    {
    case 'e': return 2;
    case 'w': return 2;
    default: break;
    }
  return 1;
}

#define CONSTRAINT_LEN(c_,s_) insn_constraint_len (c_,s_)

extern enum reg_class regclass_for_constraint (enum constraint_num);
#define REG_CLASS_FROM_CONSTRAINT(c_,s_) \
    regclass_for_constraint (lookup_constraint (s_))
#define REG_CLASS_FOR_CONSTRAINT(x_) \
    regclass_for_constraint (x_)

extern bool insn_const_int_ok_for_constraint (HOST_WIDE_INT, enum constraint_num);
#define CONST_OK_FOR_CONSTRAINT_P(v_,c_,s_) \
    insn_const_int_ok_for_constraint (v_, lookup_constraint (s_))

#define CONST_DOUBLE_OK_FOR_CONSTRAINT_P(v_,c_,s_) \
    constraint_satisfied_p (v_, lookup_constraint (s_))

#define EXTRA_CONSTRAINT_STR(v_,c_,s_) \
    constraint_satisfied_p (v_, lookup_constraint (s_))

extern bool insn_extra_memory_constraint (enum constraint_num);
#define EXTRA_MEMORY_CONSTRAINT(c_,s_) insn_extra_memory_constraint (lookup_constraint (s_))

extern bool insn_extra_address_constraint (enum constraint_num);
#define EXTRA_ADDRESS_CONSTRAINT(c_,s_) insn_extra_address_constraint (lookup_constraint (s_))

#endif /* tm-preds.h */
