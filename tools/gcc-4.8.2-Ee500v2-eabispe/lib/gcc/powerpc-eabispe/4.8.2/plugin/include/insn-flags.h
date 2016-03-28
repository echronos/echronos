/* Generated automatically by the program `genflags'
   from the machine description file `md'.  */

#ifndef GCC_INSN_FLAGS_H
#define GCC_INSN_FLAGS_H

#define HAVE_load_macho_picbase_si ((DEFAULT_ABI == ABI_DARWIN) && flag_pic)
#define HAVE_load_macho_picbase_di ((DEFAULT_ABI == ABI_DARWIN) && flag_pic && TARGET_64BIT)
#define HAVE_macho_correct_pic_si (DEFAULT_ABI == ABI_DARWIN)
#define HAVE_macho_correct_pic_di (DEFAULT_ABI == ABI_DARWIN && TARGET_64BIT)
#define HAVE_extendqidi2 (TARGET_POWERPC64)
#define HAVE_extendqisi2 1
#define HAVE_extendqihi2 1
#define HAVE_dlmzb (TARGET_DLMZB)
#define HAVE_one_cmplsi2 1
#define HAVE_one_cmpldi2 (TARGET_POWERPC64)
#define HAVE_clzsi2 1
#define HAVE_clzdi2 (TARGET_POWERPC64)
#define HAVE_popcntbsi2 (TARGET_POPCNTB)
#define HAVE_popcntbdi2 ((TARGET_POPCNTB) && (TARGET_POWERPC64))
#define HAVE_popcntdsi2 (TARGET_POPCNTD)
#define HAVE_popcntddi2 ((TARGET_POPCNTD) && (TARGET_POWERPC64))
#define HAVE_paritysi2_cmpb (TARGET_CMPB && TARGET_POPCNTB)
#define HAVE_paritydi2_cmpb ((TARGET_CMPB && TARGET_POPCNTB) && (TARGET_POWERPC64))
#define HAVE_bswaphi2_internal 1
#define HAVE_bswapdi2_32bit (!TARGET_POWERPC64 && (REG_P (operands[0]) || REG_P (operands[1])))
#define HAVE_mulsi3 1
#define HAVE_udivsi3 1
#define HAVE_udivdi3 (TARGET_POWERPC64)
#define HAVE_andsi3_mc (rs6000_gen_cell_microcode)
#define HAVE_andsi3_nomc (!rs6000_gen_cell_microcode)
#define HAVE_andsi3_internal0_nomc (!rs6000_gen_cell_microcode)
#define HAVE_insvsi_internal 1
#define HAVE_insvdi_internal (TARGET_POWERPC64)
#define HAVE_extzvsi_internal 1
#define HAVE_extzvdi_internal (TARGET_POWERPC64)
#define HAVE_rotlsi3 1
#define HAVE_ashlsi3 1
#define HAVE_rlwinm (includes_lshift_p (operands[2], operands[3]))
#define HAVE_lshrsi3 1
#define HAVE_ashrsi3 1
#define HAVE_fres (TARGET_FRES)
#define HAVE_copysignsf3_fcpsgn (TARGET_LFIWAX && !VECTOR_UNIT_VSX_P (SFmode))
#define HAVE_copysigndf3_fcpsgn (TARGET_LFIWAX && !VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_isel_signed_si (TARGET_ISEL)
#define HAVE_isel_signed_di ((TARGET_ISEL64) && (TARGET_POWERPC64))
#define HAVE_isel_unsigned_si (TARGET_ISEL)
#define HAVE_isel_unsigned_di ((TARGET_ISEL64) && (TARGET_POWERPC64))
#define HAVE_lfiwax (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWAX)
#define HAVE_floatsisf2_lfiwax (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWAX \
   && TARGET_FCFIDS && can_create_pseudo_p ())
#define HAVE_floatsidf2_lfiwax (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWAX \
   && TARGET_FCFID && can_create_pseudo_p ())
#define HAVE_floatsisf2_lfiwax_mem (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWAX \
   && TARGET_FCFIDS)
#define HAVE_floatsidf2_lfiwax_mem (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWAX \
   && TARGET_FCFID)
#define HAVE_lfiwzx (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWZX)
#define HAVE_floatunssisf2_lfiwzx (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWZX \
   && TARGET_FCFIDS)
#define HAVE_floatunssidf2_lfiwzx (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWZX \
   && TARGET_FCFID)
#define HAVE_floatunssisf2_lfiwzx_mem (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWZX \
   && TARGET_FCFIDS)
#define HAVE_floatunssidf2_lfiwzx_mem (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_LFIWZX \
   && TARGET_FCFID)
#define HAVE_fix_truncsfsi2_stfiwx (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT \
   && (SFmode != SFmode || TARGET_SINGLE_FLOAT) \
   && TARGET_STFIWX && can_create_pseudo_p ())
#define HAVE_fix_truncdfsi2_stfiwx (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT \
   && (DFmode != SFmode || TARGET_SINGLE_FLOAT) \
   && TARGET_STFIWX && can_create_pseudo_p ())
#define HAVE_fix_truncsfsi2_internal (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_fix_truncdfsi2_internal (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_fixuns_truncsfsi2_stfiwx (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT && TARGET_FCTIWUZ \
   && TARGET_STFIWX && can_create_pseudo_p ())
#define HAVE_fixuns_truncdfsi2_stfiwx (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_FCTIWUZ \
   && TARGET_STFIWX && can_create_pseudo_p ())
#define HAVE_fctiwz_sf (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_fctiwz_df (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_fctiwuz_sf (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT && TARGET_FCTIWUZ)
#define HAVE_fctiwuz_df (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_FCTIWUZ)
#define HAVE_lrintsfdi2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_lrintdfdi2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_roundsf2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_rounddf2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_stfiwx (TARGET_PPC_GFXOPT)
#define HAVE_floatdisf2_fcfids (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT \
   && TARGET_DOUBLE_FLOAT && TARGET_FCFIDS)
#define HAVE_floatdisf2_internal1 (TARGET_FCFID && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_floatunsdisf2_fcfidus (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT \
   && TARGET_DOUBLE_FLOAT && TARGET_FCFIDUS)
#define HAVE_mulsidi3 (! TARGET_POWERPC64)
#define HAVE_umulsidi3 (! TARGET_POWERPC64)
#define HAVE_smulsi3_highpart 1
#define HAVE_umulsi3_highpart 1
#define HAVE_ashrdi3_no_power (!TARGET_POWERPC64)
#define HAVE_muldi3 (TARGET_POWERPC64)
#define HAVE_smuldi3_highpart (TARGET_POWERPC64)
#define HAVE_umuldi3_highpart (TARGET_POWERPC64)
#define HAVE_rotldi3 (TARGET_POWERPC64)
#define HAVE_ashldi3_internal5 (TARGET_64BIT && includes_rldic_lshift_p (operands[2], operands[3]))
#define HAVE_ashldi3_internal8 (TARGET_64BIT && includes_rldicr_lshift_p (operands[2], operands[3]))
#define HAVE_anddi3_mc (TARGET_POWERPC64 && rs6000_gen_cell_microcode)
#define HAVE_anddi3_nomc (TARGET_POWERPC64 && !rs6000_gen_cell_microcode)
#define HAVE_trunctfdf2_internal1 (!TARGET_IEEEQUAD && !TARGET_XL_COMPAT \
   && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_LONG_DOUBLE_128)
#define HAVE_trunctfdf2_internal2 (!TARGET_IEEEQUAD && TARGET_XL_COMPAT \
   && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT  \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_trunctfsf2_fprs (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT  \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_fix_trunc_helper (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_negtf2_internal (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_LONG_DOUBLE_128)
#define HAVE_movdi_si_update ((TARGET_POWERPC64 && TARGET_UPDATE \
   && (!avoiding_indexed_address_p (Pmode) \
       || !gpc_reg_operand (operands[2], Pmode) \
       || (REG_P (operands[0]) \
	   && REGNO (operands[0]) == STACK_POINTER_REGNUM))) && (TARGET_32BIT))
#define HAVE_movdi_di_update ((TARGET_POWERPC64 && TARGET_UPDATE \
   && (!avoiding_indexed_address_p (Pmode) \
       || !gpc_reg_operand (operands[2], Pmode) \
       || (REG_P (operands[0]) \
	   && REGNO (operands[0]) == STACK_POINTER_REGNUM))) && (TARGET_64BIT))
#define HAVE_movdi_si_update_stack ((TARGET_POWERPC64) && (TARGET_32BIT))
#define HAVE_movdi_di_update_stack ((TARGET_POWERPC64) && (TARGET_64BIT))
#define HAVE_movsi_update (TARGET_UPDATE \
   && (!avoiding_indexed_address_p (SImode) \
       || !gpc_reg_operand (operands[2], SImode) \
       || (REG_P (operands[0]) \
	   && REGNO (operands[0]) == STACK_POINTER_REGNUM)))
#define HAVE_movsi_update_stack 1
#define HAVE_tls_gd_aix32 ((HAVE_AS_TLS && DEFAULT_ABI == ABI_AIX) && (! TARGET_64BIT))
#define HAVE_tls_gd_aix64 ((HAVE_AS_TLS && DEFAULT_ABI == ABI_AIX) && (TARGET_64BIT))
#define HAVE_tls_gd_sysvsi ((HAVE_AS_TLS && DEFAULT_ABI == ABI_V4) && (! TARGET_64BIT))
#define HAVE_tls_gd_sysvdi ((HAVE_AS_TLS && DEFAULT_ABI == ABI_V4) && (TARGET_64BIT))
#define HAVE_tls_ld_aix32 ((HAVE_AS_TLS && DEFAULT_ABI == ABI_AIX) && (! TARGET_64BIT))
#define HAVE_tls_ld_aix64 ((HAVE_AS_TLS && DEFAULT_ABI == ABI_AIX) && (TARGET_64BIT))
#define HAVE_tls_ld_sysvsi ((HAVE_AS_TLS && DEFAULT_ABI == ABI_V4) && (! TARGET_64BIT))
#define HAVE_tls_ld_sysvdi ((HAVE_AS_TLS && DEFAULT_ABI == ABI_V4) && (TARGET_64BIT))
#define HAVE_tls_dtprel_32 ((HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_dtprel_64 ((HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_tls_dtprel_ha_32 ((HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_dtprel_ha_64 ((HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_tls_dtprel_lo_32 ((HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_dtprel_lo_64 ((HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_tls_got_dtprel_32 ((HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_got_dtprel_64 ((HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_tls_tprel_32 ((HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_tprel_64 ((HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_tls_tprel_ha_32 ((HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_tprel_ha_64 ((HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_tls_tprel_lo_32 ((HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_tprel_lo_64 ((HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_tls_got_tprel_32 ((HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_got_tprel_64 ((HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_tls_tls_32 ((TARGET_ELF && HAVE_AS_TLS) && (! TARGET_64BIT))
#define HAVE_tls_tls_64 ((TARGET_ELF && HAVE_AS_TLS) && (TARGET_64BIT))
#define HAVE_load_toc_aix_si (DEFAULT_ABI == ABI_AIX && TARGET_32BIT)
#define HAVE_load_toc_aix_di (DEFAULT_ABI == ABI_AIX && TARGET_64BIT)
#define HAVE_load_toc_v4_pic_si (DEFAULT_ABI == ABI_V4 && flag_pic == 1 && TARGET_32BIT)
#define HAVE_load_toc_v4_PIC_1_normal (!TARGET_LINK_STACK && TARGET_ELF && DEFAULT_ABI != ABI_AIX \
   && (flag_pic == 2 || (flag_pic && TARGET_SECURE_PLT)))
#define HAVE_load_toc_v4_PIC_1b_normal (!TARGET_LINK_STACK && TARGET_ELF && DEFAULT_ABI != ABI_AIX && flag_pic == 2)
#define HAVE_load_toc_v4_PIC_2 (TARGET_ELF && DEFAULT_ABI != ABI_AIX && flag_pic == 2)
#define HAVE_load_toc_v4_PIC_3b (TARGET_ELF && TARGET_SECURE_PLT && DEFAULT_ABI != ABI_AIX && flag_pic)
#define HAVE_load_toc_v4_PIC_3c (TARGET_ELF && TARGET_SECURE_PLT && DEFAULT_ABI != ABI_AIX && flag_pic)
#define HAVE_elf_high (TARGET_ELF && ! TARGET_64BIT)
#define HAVE_elf_low (TARGET_ELF && ! TARGET_64BIT)
#define HAVE_call_indirect_aix32bit ((DEFAULT_ABI == ABI_AIX && TARGET_POINTERS_TO_NESTED_FUNCTIONS) && (TARGET_32BIT))
#define HAVE_call_indirect_aix64bit ((DEFAULT_ABI == ABI_AIX && TARGET_POINTERS_TO_NESTED_FUNCTIONS) && (TARGET_64BIT))
#define HAVE_call_indirect_aix32bit_nor11 ((DEFAULT_ABI == ABI_AIX && !TARGET_POINTERS_TO_NESTED_FUNCTIONS) && (TARGET_32BIT))
#define HAVE_call_indirect_aix64bit_nor11 ((DEFAULT_ABI == ABI_AIX && !TARGET_POINTERS_TO_NESTED_FUNCTIONS) && (TARGET_64BIT))
#define HAVE_call_value_indirect_aix32bit ((DEFAULT_ABI == ABI_AIX && TARGET_POINTERS_TO_NESTED_FUNCTIONS) && (TARGET_32BIT))
#define HAVE_call_value_indirect_aix64bit ((DEFAULT_ABI == ABI_AIX && TARGET_POINTERS_TO_NESTED_FUNCTIONS) && (TARGET_64BIT))
#define HAVE_call_value_indirect_aix32bit_nor11 ((DEFAULT_ABI == ABI_AIX && !TARGET_POINTERS_TO_NESTED_FUNCTIONS) && (TARGET_32BIT))
#define HAVE_call_value_indirect_aix64bit_nor11 ((DEFAULT_ABI == ABI_AIX && !TARGET_POINTERS_TO_NESTED_FUNCTIONS) && (TARGET_64BIT))
#define HAVE_blockage 1
#define HAVE_probe_stack_si (TARGET_32BIT)
#define HAVE_probe_stack_di (TARGET_64BIT)
#define HAVE_probe_stack_rangesi (TARGET_32BIT)
#define HAVE_probe_stack_rangedi (TARGET_64BIT)
#define HAVE_stack_protect_setsi (TARGET_32BIT)
#define HAVE_stack_protect_setdi (TARGET_64BIT)
#define HAVE_stack_protect_testsi (TARGET_32BIT)
#define HAVE_stack_protect_testdi (TARGET_64BIT)
#define HAVE_move_from_CR_gt_bit (TARGET_HARD_FLOAT && !TARGET_FPRS)
#define HAVE_move_from_CR_ov_bit (TARGET_ISEL)
#define HAVE_jump 1
#define HAVE_return (direct_return ())
#define HAVE_simple_return 1
#define HAVE_nop 1
#define HAVE_group_ending_nop 1
#define HAVE_trap 1
#define HAVE_movesi_from_cr 1
#define HAVE_stack_tie 1
#define HAVE_eh_set_lr_si (TARGET_32BIT)
#define HAVE_eh_set_lr_di (TARGET_64BIT)
#define HAVE_prefetch 1
#define HAVE_bpermd_si ((TARGET_POPCNTD) && (TARGET_32BIT))
#define HAVE_bpermd_di ((TARGET_POPCNTD) && (TARGET_64BIT))
#define HAVE_rs6000_get_timebase_ppc32 (!TARGET_POWERPC64)
#define HAVE_rs6000_mftb_si (TARGET_32BIT)
#define HAVE_rs6000_mftb_di (TARGET_64BIT)
#define HAVE_isync 1
#define HAVE_loadsync_qi 1
#define HAVE_loadsync_hi 1
#define HAVE_loadsync_si 1
#define HAVE_loadsync_di (TARGET_POWERPC64)
#define HAVE_load_lockedsi 1
#define HAVE_load_lockeddi (TARGET_POWERPC64)
#define HAVE_store_conditionalsi 1
#define HAVE_store_conditionaldi (TARGET_POWERPC64)
#define HAVE_vsx_fredf2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_frev4sf2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_frev2df2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_nabsdf2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_nabsv4sf2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_nabsv2df2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_smaxdf3 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_smaxv4sf3 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_smaxv2df3 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_eqv4sf (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_eqv2df (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_gtv4sf (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_gtv2df (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_copysigndf3 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_copysignv4sf3 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_copysignv2df3 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_floatdidf2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_floatv4siv4sf2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_floatv2div2df2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_floatunsdidf2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_floatunsv4siv4sf2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_floatunsv2div2df2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_fix_truncdfdi2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_fix_truncv4sfv4si2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_fix_truncv2dfv2di2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_fixuns_truncdfdi2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_fixuns_truncv4sfv4si2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_fixuns_truncv2dfv2di2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xsrdpi (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_xvrspi (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_xvrdpi (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xsrdpic (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_xvrspic (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_xvrdpic (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_btruncdf2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_btruncv4sf2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_btruncv2df2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_floordf2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_floorv4sf2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_floorv2df2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_ceildf2 (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_ceilv4sf2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_ceilv2df2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xscvdpsp (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_xvcvspdp (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_xvcvdpsp (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xscvspdp (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_xscvdpsp_scalar (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_xscvspdp_scalar2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_xvcvdpsxws (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xvcvdpuxws (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xvcvsxdsp (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xvcvuxdsp (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xvcvsxwdp (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xvcvuxwdp (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xvcvspsxds (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xvcvspuxds (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_concat_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_concat_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_concat_v2sf (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_set_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_set_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_extract_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_extract_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_extract_v4sf (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_xxpermdi2_v2df_1 (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_xxpermdi2_v2di_1 (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_splat_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_splat_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_xxspltw_v4sf (VECTOR_MEM_VSX_P (V4SFmode))
#define HAVE_vsx_xxspltw_v4si (VECTOR_MEM_VSX_P (V4SImode))
#define HAVE_vsx_xxmrghw_v4sf (VECTOR_MEM_VSX_P (V4SFmode))
#define HAVE_vsx_xxmrghw_v4si (VECTOR_MEM_VSX_P (V4SImode))
#define HAVE_vsx_xxmrglw_v4sf (VECTOR_MEM_VSX_P (V4SFmode))
#define HAVE_vsx_xxmrglw_v4si (VECTOR_MEM_VSX_P (V4SImode))
#define HAVE_vsx_xxsldwi_v16qi (VECTOR_MEM_VSX_P (V16QImode))
#define HAVE_vsx_xxsldwi_v8hi (VECTOR_MEM_VSX_P (V8HImode))
#define HAVE_vsx_xxsldwi_v4si (VECTOR_MEM_VSX_P (V4SImode))
#define HAVE_vsx_xxsldwi_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_xxsldwi_v4sf (VECTOR_MEM_VSX_P (V4SFmode))
#define HAVE_vsx_xxsldwi_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_xxsldwi_ti (VECTOR_MEM_VSX_P (TImode))
#define HAVE_get_vrsave_internal (TARGET_ALTIVEC)
#define HAVE_altivec_vabsduw (TARGET_ALTIVEC2)
#define HAVE_altivec_vabsduh (TARGET_ALTIVEC2)
#define HAVE_altivec_vabsdub (TARGET_ALTIVEC2)
#define HAVE_addv4si3 (TARGET_ALTIVEC)
#define HAVE_addv8hi3 (TARGET_ALTIVEC)
#define HAVE_addv16qi3 (TARGET_ALTIVEC)
#define HAVE_altivec_vaddcuw (TARGET_ALTIVEC)
#define HAVE_altivec_vadduws (TARGET_ALTIVEC)
#define HAVE_altivec_vadduhs (TARGET_ALTIVEC)
#define HAVE_altivec_vaddubs (TARGET_ALTIVEC)
#define HAVE_altivec_vaddsws (TARGET_ALTIVEC)
#define HAVE_altivec_vaddshs (TARGET_ALTIVEC)
#define HAVE_altivec_vaddsbs (TARGET_ALTIVEC)
#define HAVE_subv4si3 (TARGET_ALTIVEC)
#define HAVE_subv8hi3 (TARGET_ALTIVEC)
#define HAVE_subv16qi3 (TARGET_ALTIVEC)
#define HAVE_altivec_vsubcuw (TARGET_ALTIVEC)
#define HAVE_altivec_vsubuws (TARGET_ALTIVEC)
#define HAVE_altivec_vsubuhs (TARGET_ALTIVEC)
#define HAVE_altivec_vsububs (TARGET_ALTIVEC)
#define HAVE_altivec_vsubsws (TARGET_ALTIVEC)
#define HAVE_altivec_vsubshs (TARGET_ALTIVEC)
#define HAVE_altivec_vsubsbs (TARGET_ALTIVEC)
#define HAVE_altivec_vavguw (TARGET_ALTIVEC)
#define HAVE_altivec_vavguh (TARGET_ALTIVEC)
#define HAVE_altivec_vavgub (TARGET_ALTIVEC)
#define HAVE_altivec_vavgsw (TARGET_ALTIVEC)
#define HAVE_altivec_vavgsh (TARGET_ALTIVEC)
#define HAVE_altivec_vavgsb (TARGET_ALTIVEC)
#define HAVE_altivec_vcmpbfp (TARGET_ALTIVEC)
#define HAVE_altivec_vmsumuhm (TARGET_ALTIVEC)
#define HAVE_altivec_vmsumubm (TARGET_ALTIVEC)
#define HAVE_altivec_vmsummhm (TARGET_ALTIVEC)
#define HAVE_altivec_vmsummbm (TARGET_ALTIVEC)
#define HAVE_altivec_vmsumshm (TARGET_ALTIVEC)
#define HAVE_altivec_vmsumuhs (TARGET_ALTIVEC)
#define HAVE_altivec_vmsumshs (TARGET_ALTIVEC)
#define HAVE_umaxv4si3 (TARGET_ALTIVEC)
#define HAVE_umaxv8hi3 (TARGET_ALTIVEC)
#define HAVE_umaxv16qi3 (TARGET_ALTIVEC)
#define HAVE_smaxv4si3 (TARGET_ALTIVEC)
#define HAVE_smaxv8hi3 (TARGET_ALTIVEC)
#define HAVE_smaxv16qi3 (TARGET_ALTIVEC)
#define HAVE_uminv4si3 (TARGET_ALTIVEC)
#define HAVE_uminv8hi3 (TARGET_ALTIVEC)
#define HAVE_uminv16qi3 (TARGET_ALTIVEC)
#define HAVE_sminv4si3 (TARGET_ALTIVEC)
#define HAVE_sminv8hi3 (TARGET_ALTIVEC)
#define HAVE_sminv16qi3 (TARGET_ALTIVEC)
#define HAVE_altivec_vmhaddshs (TARGET_ALTIVEC)
#define HAVE_altivec_vmhraddshs (TARGET_ALTIVEC)
#define HAVE_altivec_vmladduhm (TARGET_ALTIVEC)
#define HAVE_altivec_vmrghb (TARGET_ALTIVEC)
#define HAVE_altivec_vmrghh (TARGET_ALTIVEC)
#define HAVE_altivec_vmrghw (VECTOR_MEM_ALTIVEC_P (V4SImode))
#define HAVE_altivec_vmrglb (TARGET_ALTIVEC)
#define HAVE_altivec_vmrglh (TARGET_ALTIVEC)
#define HAVE_altivec_vmrglw (VECTOR_MEM_ALTIVEC_P (V4SImode))
#define HAVE_vec_widen_umult_even_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_widen_smult_even_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_widen_umult_even_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_widen_smult_even_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_widen_umult_odd_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_widen_smult_odd_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_widen_umult_odd_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_widen_smult_odd_v8hi (TARGET_ALTIVEC)
#define HAVE_altivec_vpkuhum (TARGET_ALTIVEC)
#define HAVE_altivec_vpkuwum (TARGET_ALTIVEC)
#define HAVE_altivec_vpkpx (TARGET_ALTIVEC)
#define HAVE_altivec_vpkshss (TARGET_ALTIVEC)
#define HAVE_altivec_vpkswss (TARGET_ALTIVEC)
#define HAVE_altivec_vpkuhus (TARGET_ALTIVEC)
#define HAVE_altivec_vpkshus (TARGET_ALTIVEC)
#define HAVE_altivec_vpkuwus (TARGET_ALTIVEC)
#define HAVE_altivec_vpkswus (TARGET_ALTIVEC)
#define HAVE_altivec_vsl (TARGET_ALTIVEC)
#define HAVE_altivec_vslo (TARGET_ALTIVEC)
#define HAVE_altivec_vsr (TARGET_ALTIVEC)
#define HAVE_altivec_vsro (TARGET_ALTIVEC)
#define HAVE_altivec_vsum4ubs (TARGET_ALTIVEC)
#define HAVE_altivec_vsum4shs (TARGET_ALTIVEC)
#define HAVE_altivec_vsum4sbs (TARGET_ALTIVEC)
#define HAVE_altivec_vsum2sws (TARGET_ALTIVEC)
#define HAVE_altivec_vsumsws (TARGET_ALTIVEC)
#define HAVE_altivec_vspltb (TARGET_ALTIVEC)
#define HAVE_altivec_vsplth (TARGET_ALTIVEC)
#define HAVE_altivec_vspltw (TARGET_ALTIVEC)
#define HAVE_altivec_vspltsf (VECTOR_UNIT_ALTIVEC_P (V4SFmode))
#define HAVE_altivec_vspltisw (TARGET_ALTIVEC)
#define HAVE_altivec_vspltish (TARGET_ALTIVEC)
#define HAVE_altivec_vspltisb (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v4si (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v8hi (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v16qi (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v4sf (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v2df (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v2di (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_ti (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v4si_uns (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v8hi_uns (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v16qi_uns (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v4sf_uns (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v2df_uns (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_v2di_uns (TARGET_ALTIVEC)
#define HAVE_altivec_vperm_ti_uns (TARGET_ALTIVEC)
#define HAVE_altivec_vrfip (TARGET_ALTIVEC)
#define HAVE_altivec_vrfin (TARGET_ALTIVEC)
#define HAVE_altivec_vcfux (TARGET_ALTIVEC)
#define HAVE_altivec_vcfsx (TARGET_ALTIVEC)
#define HAVE_altivec_vctuxs (TARGET_ALTIVEC)
#define HAVE_altivec_vctsxs (TARGET_ALTIVEC)
#define HAVE_altivec_vlogefp (TARGET_ALTIVEC)
#define HAVE_altivec_vexptefp (TARGET_ALTIVEC)
#define HAVE_altivec_vrefp (VECTOR_UNIT_ALTIVEC_P (V4SFmode))
#define HAVE_altivec_vsldoi_v4si (TARGET_ALTIVEC)
#define HAVE_altivec_vsldoi_v8hi (TARGET_ALTIVEC)
#define HAVE_altivec_vsldoi_v16qi (TARGET_ALTIVEC)
#define HAVE_altivec_vsldoi_v4sf (TARGET_ALTIVEC)
#define HAVE_altivec_vsldoi_v2df (TARGET_ALTIVEC)
#define HAVE_altivec_vsldoi_v2di (TARGET_ALTIVEC)
#define HAVE_altivec_vsldoi_ti (TARGET_ALTIVEC)
#define HAVE_altivec_vupkhsb (TARGET_ALTIVEC)
#define HAVE_altivec_vupkhpx (TARGET_ALTIVEC)
#define HAVE_altivec_vupkhsh (TARGET_ALTIVEC)
#define HAVE_altivec_vupklsb (TARGET_ALTIVEC)
#define HAVE_altivec_vupklpx (TARGET_ALTIVEC)
#define HAVE_altivec_vupklsh (TARGET_ALTIVEC)
#define HAVE_altivec_vcmpbfp_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_altivec_mtvscr (TARGET_ALTIVEC)
#define HAVE_altivec_mfvscr (TARGET_ALTIVEC)
#define HAVE_altivec_dssall (TARGET_ALTIVEC)
#define HAVE_altivec_dss (TARGET_ALTIVEC)
#define HAVE_altivec_dst (TARGET_ALTIVEC && GET_MODE (operands[0]) == Pmode)
#define HAVE_altivec_dstt (TARGET_ALTIVEC && GET_MODE (operands[0]) == Pmode)
#define HAVE_altivec_dstst (TARGET_ALTIVEC && GET_MODE (operands[0]) == Pmode)
#define HAVE_altivec_dststt (TARGET_ALTIVEC && GET_MODE (operands[0]) == Pmode)
#define HAVE_altivec_lvsl (TARGET_ALTIVEC)
#define HAVE_altivec_lvsr (TARGET_ALTIVEC)
#define HAVE_altivec_lvewx (TARGET_ALTIVEC)
#define HAVE_altivec_lvehx (TARGET_ALTIVEC)
#define HAVE_altivec_lvebx (TARGET_ALTIVEC)
#define HAVE_altivec_lvexwx (TARGET_ALTIVEC2)
#define HAVE_altivec_lvexhx (TARGET_ALTIVEC2)
#define HAVE_altivec_lvexbx (TARGET_ALTIVEC2)
#define HAVE_altivec_lvxl (TARGET_ALTIVEC)
#define HAVE_altivec_lvx_v4si (TARGET_ALTIVEC)
#define HAVE_altivec_lvx_v8hi (TARGET_ALTIVEC)
#define HAVE_altivec_lvx_v16qi (TARGET_ALTIVEC)
#define HAVE_altivec_lvx_v4sf (TARGET_ALTIVEC)
#define HAVE_altivec_lvx_v2df (TARGET_ALTIVEC)
#define HAVE_altivec_lvx_v2di (TARGET_ALTIVEC)
#define HAVE_altivec_stvx_v4si (TARGET_ALTIVEC)
#define HAVE_altivec_stvx_v8hi (TARGET_ALTIVEC)
#define HAVE_altivec_stvx_v16qi (TARGET_ALTIVEC)
#define HAVE_altivec_stvx_v4sf (TARGET_ALTIVEC)
#define HAVE_altivec_stvx_v2df (TARGET_ALTIVEC)
#define HAVE_altivec_stvx_v2di (TARGET_ALTIVEC)
#define HAVE_altivec_stvxl (TARGET_ALTIVEC)
#define HAVE_altivec_stvewx (TARGET_ALTIVEC)
#define HAVE_altivec_stvehx (TARGET_ALTIVEC)
#define HAVE_altivec_stvebx (TARGET_ALTIVEC)
#define HAVE_altivec_stvexwx (TARGET_ALTIVEC2)
#define HAVE_altivec_stvexhx (TARGET_ALTIVEC2)
#define HAVE_altivec_stvexbx (TARGET_ALTIVEC2)
#define HAVE_vperm_v8hiv4si (TARGET_ALTIVEC)
#define HAVE_vperm_v16qiv8hi (TARGET_ALTIVEC)
#define HAVE_altivec_lvlx (TARGET_ALTIVEC && rs6000_cpu == PROCESSOR_CELL)
#define HAVE_altivec_lvlxl (TARGET_ALTIVEC && rs6000_cpu == PROCESSOR_CELL)
#define HAVE_altivec_lvrx (TARGET_ALTIVEC && rs6000_cpu == PROCESSOR_CELL)
#define HAVE_altivec_lvrxl (TARGET_ALTIVEC && rs6000_cpu == PROCESSOR_CELL)
#define HAVE_altivec_stvlx (TARGET_ALTIVEC && rs6000_cpu == PROCESSOR_CELL)
#define HAVE_altivec_stvlxl (TARGET_ALTIVEC && rs6000_cpu == PROCESSOR_CELL)
#define HAVE_altivec_stvrx (TARGET_ALTIVEC && rs6000_cpu == PROCESSOR_CELL)
#define HAVE_altivec_stvrxl (TARGET_ALTIVEC && rs6000_cpu == PROCESSOR_CELL)
#define HAVE_altivec_lvtlx (TARGET_ALTIVEC2)
#define HAVE_altivec_lvtlxl (TARGET_ALTIVEC2)
#define HAVE_altivec_lvtrx (TARGET_ALTIVEC2)
#define HAVE_altivec_lvtrxl (TARGET_ALTIVEC2)
#define HAVE_altivec_stvflx (TARGET_ALTIVEC2)
#define HAVE_altivec_stvflxl (TARGET_ALTIVEC2)
#define HAVE_altivec_stvfrx (TARGET_ALTIVEC2)
#define HAVE_altivec_stvfrxl (TARGET_ALTIVEC2)
#define HAVE_altivec_lvswx (TARGET_ALTIVEC2)
#define HAVE_altivec_lvswxl (TARGET_ALTIVEC2)
#define HAVE_altivec_lvsm (TARGET_ALTIVEC2)
#define HAVE_altivec_stvswx (TARGET_ALTIVEC2)
#define HAVE_altivec_stvswxl (TARGET_ALTIVEC2)
#define HAVE_spe_fixuns_truncdfsi2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_extendsfdf2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_fixuns_truncsfsi2 (TARGET_HARD_FLOAT && !TARGET_FPRS)
#define HAVE_spe_fix_truncsfsi2 (TARGET_HARD_FLOAT && !TARGET_FPRS)
#define HAVE_spe_fix_truncdfsi2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_floatunssisf2 (TARGET_HARD_FLOAT && !TARGET_FPRS)
#define HAVE_spe_floatunssidf2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_floatsisf2 (TARGET_HARD_FLOAT && !TARGET_FPRS)
#define HAVE_spe_floatsidf2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_absv2si2 (TARGET_SPE)
#define HAVE_spe_evandc (TARGET_SPE)
#define HAVE_andv2si3 (TARGET_SPE)
#define HAVE_spe_evcmpeq (TARGET_SPE)
#define HAVE_spe_evcmpgts (TARGET_SPE)
#define HAVE_spe_evcmpgtu (TARGET_SPE)
#define HAVE_spe_evcmplts (TARGET_SPE)
#define HAVE_spe_evcmpltu (TARGET_SPE)
#define HAVE_spe_evfscmpeq (TARGET_SPE)
#define HAVE_spe_evfscmpgt (TARGET_SPE)
#define HAVE_spe_evfscmplt (TARGET_SPE)
#define HAVE_spe_evfststeq (TARGET_SPE)
#define HAVE_spe_evfststgt (TARGET_SPE)
#define HAVE_spe_evfststlt (TARGET_SPE)
#define HAVE_spe_evcntlsw (TARGET_SPE)
#define HAVE_spe_evcntlzw (TARGET_SPE)
#define HAVE_spe_eveqv (TARGET_SPE)
#define HAVE_spe_evextsb (TARGET_SPE)
#define HAVE_spe_evextsh (TARGET_SPE)
#define HAVE_spe_evlhhesplat (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlhhesplatx (TARGET_SPE)
#define HAVE_spe_evlhhossplat (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlhhossplatx (TARGET_SPE)
#define HAVE_spe_evlhhousplat (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlhhousplatx (TARGET_SPE)
#define HAVE_spe_evlwhsplat (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlwhsplatx (TARGET_SPE)
#define HAVE_spe_evlwwsplat (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlwwsplatx (TARGET_SPE)
#define HAVE_spe_evmergehi (TARGET_SPE)
#define HAVE_spe_evmergehilo (TARGET_SPE)
#define HAVE_spe_evmergelo (TARGET_SPE)
#define HAVE_spe_evmergelohi (TARGET_SPE)
#define HAVE_spe_evnand (TARGET_SPE)
#define HAVE_negv2si2 (TARGET_SPE)
#define HAVE_spe_evnor (TARGET_SPE)
#define HAVE_spe_evorc (TARGET_SPE)
#define HAVE_spe_evor (TARGET_SPE)
#define HAVE_spe_evrlwi (TARGET_SPE)
#define HAVE_spe_evrlw (TARGET_SPE)
#define HAVE_spe_evrndw (TARGET_SPE)
#define HAVE_spe_evsel (TARGET_SPE)
#define HAVE_spe_evsel_fs (TARGET_SPE)
#define HAVE_spe_evslwi (TARGET_SPE)
#define HAVE_spe_evslw (TARGET_SPE)
#define HAVE_spe_evsrwis (TARGET_SPE)
#define HAVE_spe_evsrwiu (TARGET_SPE)
#define HAVE_spe_evsrws (TARGET_SPE)
#define HAVE_spe_evsrwu (TARGET_SPE)
#define HAVE_xorv2si3 (TARGET_SPE)
#define HAVE_xorv4hi3 (TARGET_SPE)
#define HAVE_xorv1di3 (TARGET_SPE)
#define HAVE_spe_evfsabs (TARGET_SPE)
#define HAVE_spe_evfsadd (TARGET_SPE)
#define HAVE_spe_evfscfsf (TARGET_SPE)
#define HAVE_spe_evfscfsi (TARGET_SPE)
#define HAVE_spe_evfscfuf (TARGET_SPE)
#define HAVE_spe_evfscfui (TARGET_SPE)
#define HAVE_spe_evfsctsf (TARGET_SPE)
#define HAVE_spe_evfsctsi (TARGET_SPE)
#define HAVE_spe_evfsctsiz (TARGET_SPE)
#define HAVE_spe_evfsctuf (TARGET_SPE)
#define HAVE_spe_evfsctui (TARGET_SPE)
#define HAVE_spe_evfsctuiz (TARGET_SPE)
#define HAVE_spe_evfsdiv (TARGET_SPE)
#define HAVE_spe_evfsmul (TARGET_SPE)
#define HAVE_spe_evfsnabs (TARGET_SPE)
#define HAVE_spe_evfsneg (TARGET_SPE)
#define HAVE_spe_evfssub (TARGET_SPE)
#define HAVE_spe_evldd (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlddx (TARGET_SPE)
#define HAVE_spe_evldh (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evldhx (TARGET_SPE)
#define HAVE_spe_evldw (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evldwx (TARGET_SPE)
#define HAVE_spe_evlwhe (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlwhex (TARGET_SPE)
#define HAVE_spe_evlwhos (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlwhosx (TARGET_SPE)
#define HAVE_spe_evlwhou (TARGET_SPE && INTVAL (operands[2]) >= 0 && INTVAL (operands[2]) <= 31)
#define HAVE_spe_evlwhoux (TARGET_SPE)
#define HAVE_spe_brinc (TARGET_SPE)
#define HAVE_spe_evmhegsmfaa (TARGET_SPE)
#define HAVE_spe_evmhegsmfan (TARGET_SPE)
#define HAVE_spe_evmhegsmiaa (TARGET_SPE)
#define HAVE_spe_evmhegsmian (TARGET_SPE)
#define HAVE_spe_evmhegumiaa (TARGET_SPE)
#define HAVE_spe_evmhegumian (TARGET_SPE)
#define HAVE_spe_evmhesmfaaw (TARGET_SPE)
#define HAVE_spe_evmhesmfanw (TARGET_SPE)
#define HAVE_spe_evmhesmfa (TARGET_SPE)
#define HAVE_spe_evmhesmf (TARGET_SPE)
#define HAVE_spe_evmhesmiaaw (TARGET_SPE)
#define HAVE_spe_evmhesmianw (TARGET_SPE)
#define HAVE_spe_evmhesmia (TARGET_SPE)
#define HAVE_spe_evmhesmi (TARGET_SPE)
#define HAVE_spe_evmhessfaaw (TARGET_SPE)
#define HAVE_spe_evmhessfanw (TARGET_SPE)
#define HAVE_spe_evmhessfa (TARGET_SPE)
#define HAVE_spe_evmhessf (TARGET_SPE)
#define HAVE_spe_evmhessiaaw (TARGET_SPE)
#define HAVE_spe_evmhessianw (TARGET_SPE)
#define HAVE_spe_evmheumiaaw (TARGET_SPE)
#define HAVE_spe_evmheumianw (TARGET_SPE)
#define HAVE_spe_evmheumia (TARGET_SPE)
#define HAVE_spe_evmheumi (TARGET_SPE)
#define HAVE_spe_evmheusiaaw (TARGET_SPE)
#define HAVE_spe_evmheusianw (TARGET_SPE)
#define HAVE_spe_evmhogsmfaa (TARGET_SPE)
#define HAVE_spe_evmhogsmfan (TARGET_SPE)
#define HAVE_spe_evmhogsmiaa (TARGET_SPE)
#define HAVE_spe_evmhogsmian (TARGET_SPE)
#define HAVE_spe_evmhogumiaa (TARGET_SPE)
#define HAVE_spe_evmhogumian (TARGET_SPE)
#define HAVE_spe_evmhosmfaaw (TARGET_SPE)
#define HAVE_spe_evmhosmfanw (TARGET_SPE)
#define HAVE_spe_evmhosmfa (TARGET_SPE)
#define HAVE_spe_evmhosmf (TARGET_SPE)
#define HAVE_spe_evmhosmiaaw (TARGET_SPE)
#define HAVE_spe_evmhosmianw (TARGET_SPE)
#define HAVE_spe_evmhosmia (TARGET_SPE)
#define HAVE_spe_evmhosmi (TARGET_SPE)
#define HAVE_spe_evmhossfaaw (TARGET_SPE)
#define HAVE_spe_evmhossfanw (TARGET_SPE)
#define HAVE_spe_evmhossfa (TARGET_SPE)
#define HAVE_spe_evmhossf (TARGET_SPE)
#define HAVE_spe_evmhossiaaw (TARGET_SPE)
#define HAVE_spe_evmhossianw (TARGET_SPE)
#define HAVE_spe_evmhoumiaaw (TARGET_SPE)
#define HAVE_spe_evmhoumianw (TARGET_SPE)
#define HAVE_spe_evmhoumia (TARGET_SPE)
#define HAVE_spe_evmhoumi (TARGET_SPE)
#define HAVE_spe_evmhousiaaw (TARGET_SPE)
#define HAVE_spe_evmhousianw (TARGET_SPE)
#define HAVE_spe_evmmlssfa (TARGET_SPE)
#define HAVE_spe_evmmlssf (TARGET_SPE)
#define HAVE_spe_evmwhsmfa (TARGET_SPE)
#define HAVE_spe_evmwhsmf (TARGET_SPE)
#define HAVE_spe_evmwhsmia (TARGET_SPE)
#define HAVE_spe_evmwhsmi (TARGET_SPE)
#define HAVE_spe_evmwhssfa (TARGET_SPE)
#define HAVE_spe_evmwhusian (TARGET_SPE)
#define HAVE_spe_evmwhssf (TARGET_SPE)
#define HAVE_spe_evmwhumia (TARGET_SPE)
#define HAVE_spe_evmwhumi (TARGET_SPE)
#define HAVE_spe_evmwlsmiaaw (TARGET_SPE)
#define HAVE_spe_evmwlsmianw (TARGET_SPE)
#define HAVE_spe_evmwlssiaaw (TARGET_SPE)
#define HAVE_spe_evmwlssianw (TARGET_SPE)
#define HAVE_spe_evmwlumiaaw (TARGET_SPE)
#define HAVE_spe_evmwlumianw (TARGET_SPE)
#define HAVE_spe_evmwlumia (TARGET_SPE)
#define HAVE_spe_evmwlumi (TARGET_SPE)
#define HAVE_spe_evmwlusiaaw (TARGET_SPE)
#define HAVE_spe_evmwlusianw (TARGET_SPE)
#define HAVE_spe_evmwsmfaa (TARGET_SPE)
#define HAVE_spe_evmwsmfan (TARGET_SPE)
#define HAVE_spe_evmwsmfa (TARGET_SPE)
#define HAVE_spe_evmwsmf (TARGET_SPE)
#define HAVE_spe_evmwsmiaa (TARGET_SPE)
#define HAVE_spe_evmwsmian (TARGET_SPE)
#define HAVE_spe_evmwsmia (TARGET_SPE)
#define HAVE_spe_evmwsmi (TARGET_SPE)
#define HAVE_spe_evmwssfaa (TARGET_SPE)
#define HAVE_spe_evmwssfan (TARGET_SPE)
#define HAVE_spe_evmwssfa (TARGET_SPE)
#define HAVE_spe_evmwssf (TARGET_SPE)
#define HAVE_spe_evmwumiaa (TARGET_SPE)
#define HAVE_spe_evmwumian (TARGET_SPE)
#define HAVE_spe_evmwumia (TARGET_SPE)
#define HAVE_spe_evmwumi (TARGET_SPE)
#define HAVE_addv2si3 (TARGET_SPE)
#define HAVE_spe_evaddusiaaw (TARGET_SPE)
#define HAVE_spe_evaddumiaaw (TARGET_SPE)
#define HAVE_spe_evaddssiaaw (TARGET_SPE)
#define HAVE_spe_evaddsmiaaw (TARGET_SPE)
#define HAVE_spe_evaddiw (TARGET_SPE)
#define HAVE_spe_evsubifw (TARGET_SPE)
#define HAVE_subv2si3 (TARGET_SPE)
#define HAVE_spe_evsubfusiaaw (TARGET_SPE)
#define HAVE_spe_evsubfumiaaw (TARGET_SPE)
#define HAVE_spe_evsubfssiaaw (TARGET_SPE)
#define HAVE_spe_evsubfsmiaaw (TARGET_SPE)
#define HAVE_spe_evmra (TARGET_SPE)
#define HAVE_divv2si3 (TARGET_SPE)
#define HAVE_spe_evdivwu (TARGET_SPE)
#define HAVE_spe_evsplatfi (TARGET_SPE)
#define HAVE_spe_evsplati (TARGET_SPE)
#define HAVE_spe_evstdd (TARGET_SPE && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 31)
#define HAVE_spe_evstddx (TARGET_SPE)
#define HAVE_spe_evstdh (TARGET_SPE && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 31)
#define HAVE_spe_evstdhx (TARGET_SPE)
#define HAVE_spe_evstdw (TARGET_SPE && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 31)
#define HAVE_spe_evstdwx (TARGET_SPE)
#define HAVE_spe_evstwhe (TARGET_SPE && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 31)
#define HAVE_spe_evstwhex (TARGET_SPE)
#define HAVE_spe_evstwho (TARGET_SPE && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 31)
#define HAVE_spe_evstwhox (TARGET_SPE)
#define HAVE_spe_evstwwe (TARGET_SPE && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 31)
#define HAVE_spe_evstwwex (TARGET_SPE)
#define HAVE_spe_evstwwo (TARGET_SPE && INTVAL (operands[1]) >= 0 && INTVAL (operands[1]) <= 31)
#define HAVE_spe_evstwwox (TARGET_SPE)
#define HAVE_mov_sidf_e500_subreg0 ((TARGET_E500_DOUBLE && (DFmode == DFmode || DFmode == TFmode)) \
   || (TARGET_SPE && DFmode != DFmode && DFmode != TFmode))
#define HAVE_mov_siv4hi_e500_subreg0 ((TARGET_E500_DOUBLE && (V4HImode == DFmode || V4HImode == TFmode)) \
   || (TARGET_SPE && V4HImode != DFmode && V4HImode != TFmode))
#define HAVE_mov_siv2sf_e500_subreg0 ((TARGET_E500_DOUBLE && (V2SFmode == DFmode || V2SFmode == TFmode)) \
   || (TARGET_SPE && V2SFmode != DFmode && V2SFmode != TFmode))
#define HAVE_mov_siv1di_e500_subreg0 ((TARGET_E500_DOUBLE && (V1DImode == DFmode || V1DImode == TFmode)) \
   || (TARGET_SPE && V1DImode != DFmode && V1DImode != TFmode))
#define HAVE_mov_siv2si_e500_subreg0 ((TARGET_E500_DOUBLE && (V2SImode == DFmode || V2SImode == TFmode)) \
   || (TARGET_SPE && V2SImode != DFmode && V2SImode != TFmode))
#define HAVE_mov_sitf_e500_subreg0 ((TARGET_E500_DOUBLE && (TFmode == DFmode || TFmode == TFmode)) \
   || (TARGET_SPE && TFmode != DFmode && TFmode != TFmode))
#define HAVE_spe_truncdfsf2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_absdf2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_nabsdf2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_negdf2 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_adddf3 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_subdf3 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_muldf3 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_divdf3 (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE)
#define HAVE_spe_trunctfdf2_internal1 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128)
#define HAVE_spe_trunctfsf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128)
#define HAVE_spe_extenddftf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128)
#define HAVE_spe_fix_trunctfsi2_internal (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128)
#define HAVE_spe_negtf2_internal (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128)
#define HAVE_spe_evmwhssfaa (TARGET_SPE)
#define HAVE_spe_evmwhssmaa (TARGET_SPE)
#define HAVE_spe_evmwhsmfaa (TARGET_SPE)
#define HAVE_spe_evmwhsmiaa (TARGET_SPE)
#define HAVE_spe_evmwhusiaa (TARGET_SPE)
#define HAVE_spe_evmwhumiaa (TARGET_SPE)
#define HAVE_spe_evmwhssfan (TARGET_SPE)
#define HAVE_spe_evmwhssian (TARGET_SPE)
#define HAVE_spe_evmwhsmfan (TARGET_SPE)
#define HAVE_spe_evmwhsmian (TARGET_SPE)
#define HAVE_spe_evmwhumian (TARGET_SPE)
#define HAVE_spe_evmwhgssfaa (TARGET_SPE)
#define HAVE_spe_evmwhgsmfaa (TARGET_SPE)
#define HAVE_spe_evmwhgsmiaa (TARGET_SPE)
#define HAVE_spe_evmwhgumiaa (TARGET_SPE)
#define HAVE_spe_evmwhgssfan (TARGET_SPE)
#define HAVE_spe_evmwhgsmfan (TARGET_SPE)
#define HAVE_spe_evmwhgsmian (TARGET_SPE)
#define HAVE_spe_evmwhgumian (TARGET_SPE)
#define HAVE_spe_mtspefscr (TARGET_SPE)
#define HAVE_spe_mfspefscr (TARGET_SPE)
#define HAVE_e500_flip_gt_bit (!TARGET_FPRS && TARGET_HARD_FLOAT)
#define HAVE_cmpsfeq_gpr (TARGET_HARD_FLOAT && !TARGET_FPRS \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tstsfeq_gpr (TARGET_HARD_FLOAT && !TARGET_FPRS \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_cmpsfgt_gpr (TARGET_HARD_FLOAT && !TARGET_FPRS \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tstsfgt_gpr (TARGET_HARD_FLOAT && !TARGET_FPRS \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_cmpsflt_gpr (TARGET_HARD_FLOAT && !TARGET_FPRS \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tstsflt_gpr (TARGET_HARD_FLOAT && !TARGET_FPRS \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_cmpdfeq_gpr (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tstdfeq_gpr (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_cmpdfgt_gpr (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tstdfgt_gpr (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_cmpdflt_gpr (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tstdflt_gpr (TARGET_HARD_FLOAT && TARGET_E500_DOUBLE \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_cmptfeq_gpr (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128 \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tsttfeq_gpr (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128 \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_cmptfgt_gpr (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128 \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tsttfgt_gpr (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128 \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_cmptflt_gpr (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128 \
   && !(flag_finite_math_only && !flag_trapping_math))
#define HAVE_tsttflt_gpr (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128 \
   && flag_finite_math_only && !flag_trapping_math)
#define HAVE_e500_cr_ior_compare (TARGET_HARD_FLOAT && !TARGET_FPRS)
#define HAVE_movsd_hardfloat ((gpc_reg_operand (operands[0], SDmode) \
   || gpc_reg_operand (operands[1], SDmode)) \
   && (TARGET_HARD_FLOAT && TARGET_FPRS))
#define HAVE_movsd_softfloat ((gpc_reg_operand (operands[0], SDmode) \
   || gpc_reg_operand (operands[1], SDmode)) \
   && (TARGET_SOFT_FLOAT || !TARGET_FPRS))
#define HAVE_movsd_store ((gpc_reg_operand (operands[0], DDmode) \
   || gpc_reg_operand (operands[1], SDmode)) \
   && TARGET_HARD_FLOAT && TARGET_FPRS)
#define HAVE_movsd_load ((gpc_reg_operand (operands[0], SDmode) \
   || gpc_reg_operand (operands[1], DDmode)) \
   && TARGET_HARD_FLOAT && TARGET_FPRS)
#define HAVE_extendsddd2 (TARGET_DFP)
#define HAVE_truncddsd2 (TARGET_DFP)
#define HAVE_extendddtd2 (TARGET_DFP)
#define HAVE_trunctddd2 (TARGET_DFP)
#define HAVE_adddd3 (TARGET_DFP)
#define HAVE_addtd3 (TARGET_DFP)
#define HAVE_subdd3 (TARGET_DFP)
#define HAVE_subtd3 (TARGET_DFP)
#define HAVE_muldd3 (TARGET_DFP)
#define HAVE_multd3 (TARGET_DFP)
#define HAVE_divdd3 (TARGET_DFP)
#define HAVE_divtd3 (TARGET_DFP)
#define HAVE_floatdidd2 (TARGET_DFP && TARGET_POPCNTD)
#define HAVE_floatditd2 (TARGET_DFP)
#define HAVE_ftruncdd2 (TARGET_DFP)
#define HAVE_fixdddi2 (TARGET_DFP)
#define HAVE_ftrunctd2 (TARGET_DFP)
#define HAVE_fixtddi2 (TARGET_DFP)
#define HAVE_load_macho_picbase ((DEFAULT_ABI == ABI_DARWIN) && flag_pic)
#define HAVE_macho_correct_pic (DEFAULT_ABI == ABI_DARWIN)
#define HAVE_zero_extendqidi2 (TARGET_POWERPC64)
#define HAVE_zero_extendhidi2 (TARGET_POWERPC64)
#define HAVE_zero_extendsidi2 (TARGET_POWERPC64)
#define HAVE_extendhidi2 (TARGET_POWERPC64)
#define HAVE_extendsidi2 (TARGET_POWERPC64)
#define HAVE_zero_extendqisi2 1
#define HAVE_zero_extendhisi2 1
#define HAVE_extendhisi2 1
#define HAVE_strlensi (TARGET_DLMZB && WORDS_BIG_ENDIAN && !optimize_size)
#define HAVE_addsi3 1
#define HAVE_adddi3 1
#define HAVE_subsi3 1
#define HAVE_subdi3 1
#define HAVE_negsi2 1
#define HAVE_negdi2 1
#define HAVE_ctzsi2 1
#define HAVE_ctzdi2 (TARGET_POWERPC64)
#define HAVE_ffssi2 1
#define HAVE_ffsdi2 (TARGET_POWERPC64)
#define HAVE_popcountsi2 (TARGET_POPCNTB || TARGET_POPCNTD)
#define HAVE_popcountdi2 ((TARGET_POPCNTB || TARGET_POPCNTD) && (TARGET_POWERPC64))
#define HAVE_paritysi2 (TARGET_POPCNTB)
#define HAVE_paritydi2 ((TARGET_POPCNTB) && (TARGET_POWERPC64))
#define HAVE_bswaphi2 1
#define HAVE_bswapsi2 1
#define HAVE_bswapdi2 1
#define HAVE_divsi3 1
#define HAVE_divdi3 (TARGET_POWERPC64)
#define HAVE_modsi3 1
#define HAVE_moddi3 (TARGET_POWERPC64)
#define HAVE_andsi3 1
#define HAVE_iorsi3 1
#define HAVE_xorsi3 1
#define HAVE_insv 1
#define HAVE_extzv 1
#define HAVE_recipsf3 (RS6000_RECIP_HAVE_RE_P (SFmode))
#define HAVE_recipdf3 (RS6000_RECIP_HAVE_RE_P (DFmode))
#define HAVE_recipv4sf3 (RS6000_RECIP_HAVE_RE_P (V4SFmode))
#define HAVE_recipv2df3 (RS6000_RECIP_HAVE_RE_P (V2DFmode))
#define HAVE_rsqrtsf2 (RS6000_RECIP_HAVE_RSQRTE_P (SFmode))
#define HAVE_rsqrtdf2 (RS6000_RECIP_HAVE_RSQRTE_P (DFmode))
#define HAVE_rsqrtv4sf2 (RS6000_RECIP_HAVE_RSQRTE_P (V4SFmode))
#define HAVE_rsqrtv2df2 (RS6000_RECIP_HAVE_RSQRTE_P (V2DFmode))
#define HAVE_extendsfdf2 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_truncdfsf2 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_negsf2 (TARGET_HARD_FLOAT && TARGET_SINGLE_FLOAT)
#define HAVE_abssf2 (TARGET_HARD_FLOAT && TARGET_SINGLE_FLOAT)
#define HAVE_addsf3 (TARGET_HARD_FLOAT && TARGET_SINGLE_FLOAT)
#define HAVE_subsf3 (TARGET_HARD_FLOAT && TARGET_SINGLE_FLOAT)
#define HAVE_mulsf3 (TARGET_HARD_FLOAT && TARGET_SINGLE_FLOAT)
#define HAVE_divsf3 (TARGET_HARD_FLOAT && TARGET_SINGLE_FLOAT && !TARGET_SIMPLE_FPU)
#define HAVE_sqrtsf2 ((TARGET_PPC_GPOPT || TARGET_XILINX_FPU) \
   && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT \
   && !TARGET_SIMPLE_FPU)
#define HAVE_signbittf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_copysignsf3 (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT \
   && ((TARGET_PPC_GFXOPT \
        && !HONOR_NANS (SFmode) \
        && !HONOR_SIGNED_ZEROS (SFmode)) \
       || TARGET_LFIWAX \
       || VECTOR_UNIT_VSX_P (SFmode)))
#define HAVE_copysigndf3 (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT \
   && ((TARGET_PPC_GFXOPT \
        && !HONOR_NANS (DFmode) \
        && !HONOR_SIGNED_ZEROS (DFmode)) \
       || TARGET_LFIWAX \
       || VECTOR_UNIT_VSX_P (DFmode)))
#define HAVE_smaxsf3 (TARGET_PPC_GFXOPT && TARGET_HARD_FLOAT && TARGET_FPRS  \
   && TARGET_SINGLE_FLOAT && !flag_trapping_math)
#define HAVE_sminsf3 (TARGET_PPC_GFXOPT && TARGET_HARD_FLOAT && TARGET_FPRS  \
   && TARGET_SINGLE_FLOAT && !flag_trapping_math)
#define HAVE_movsicc (TARGET_ISEL)
#define HAVE_movdicc ((TARGET_ISEL64) && (TARGET_POWERPC64))
#define HAVE_movsfcc (TARGET_PPC_GFXOPT && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_negdf2 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_absdf2 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_adddf3 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_subdf3 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_muldf3 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_divdf3 (TARGET_HARD_FLOAT \
   && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE) \
   && !TARGET_SIMPLE_FPU)
#define HAVE_sqrtdf2 (TARGET_PPC_GPOPT && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_smaxdf3 (TARGET_PPC_GFXOPT && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT  \
   && !flag_trapping_math)
#define HAVE_smindf3 (TARGET_PPC_GFXOPT && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT  \
   && !flag_trapping_math)
#define HAVE_movdfcc (TARGET_PPC_GFXOPT && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_floatsidf2 (TARGET_HARD_FLOAT  \
   && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_floatunssisf2 (TARGET_HARD_FLOAT && TARGET_SINGLE_FLOAT \
   && (!TARGET_FPRS \
       || (TARGET_FPRS \
	   && ((TARGET_FCFIDUS && TARGET_LFIWZX) \
	       || (TARGET_DOUBLE_FLOAT && TARGET_FCFID \
		   && (TARGET_POWERPC64 || flag_unsafe_math_optimizations))))))
#define HAVE_floatunssidf2 (TARGET_HARD_FLOAT \
   && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_fix_truncsfsi2 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_SINGLE_FLOAT) || !TARGET_FPRS))
#define HAVE_fix_truncdfsi2 (TARGET_HARD_FLOAT && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_fix_truncsfdi2 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT && TARGET_FPRS \
   && TARGET_FCFID)
#define HAVE_fix_truncdfdi2 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT && TARGET_FPRS \
   && TARGET_FCFID)
#define HAVE_fixuns_truncsfsi2 (TARGET_HARD_FLOAT \
   && ((TARGET_FPRS && TARGET_SINGLE_FLOAT && TARGET_FCTIWUZ && TARGET_STFIWX) \
       || !TARGET_FPRS))
#define HAVE_fixuns_truncdfsi2 (TARGET_HARD_FLOAT \
   && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT && TARGET_FCTIWUZ && TARGET_STFIWX) \
       || TARGET_E500_DOUBLE))
#define HAVE_fixuns_truncsfdi2 (TARGET_HARD_FLOAT && (TARGET_FCTIDUZ || VECTOR_UNIT_VSX_P (SFmode)))
#define HAVE_fixuns_truncdfdi2 (TARGET_HARD_FLOAT && (TARGET_FCTIDUZ || VECTOR_UNIT_VSX_P (DFmode)))
#define HAVE_btruncsf2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_btruncdf2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_ceilsf2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_ceildf2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_floorsf2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_floordf2 (TARGET_FPRND && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT)
#define HAVE_floatsisf2 (TARGET_HARD_FLOAT && TARGET_SINGLE_FLOAT \
   && (!TARGET_FPRS \
       || (TARGET_FPRS \
	   && ((TARGET_FCFIDS && TARGET_LFIWAX) \
	       || (TARGET_DOUBLE_FLOAT && TARGET_FCFID \
		   && (TARGET_POWERPC64 || flag_unsafe_math_optimizations))))))
#define HAVE_floatdidf2 (TARGET_FCFID && TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT && TARGET_FPRS)
#define HAVE_floatunsdidf2 (TARGET_HARD_FLOAT && (TARGET_FCFIDU || VECTOR_UNIT_VSX_P (DFmode)))
#define HAVE_floatdisf2 (TARGET_FCFID && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT \
   && (TARGET_FCFIDS || TARGET_POWERPC64 || flag_unsafe_math_optimizations))
#define HAVE_floatdisf2_internal2 (TARGET_POWERPC64 && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_floatunsdisf2 (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT \
   && TARGET_DOUBLE_FLOAT && TARGET_FCFIDUS)
#define HAVE_mulditi3 (TARGET_POWERPC64)
#define HAVE_umulditi3 (TARGET_POWERPC64)
#define HAVE_ashldi3 (TARGET_POWERPC64)
#define HAVE_lshrdi3 (TARGET_POWERPC64)
#define HAVE_ashrdi3 1
#define HAVE_anddi3 (TARGET_POWERPC64)
#define HAVE_iordi3 (TARGET_POWERPC64)
#define HAVE_xordi3 (TARGET_POWERPC64)
#define HAVE_movsi_got (DEFAULT_ABI == ABI_V4 && flag_pic == 1)
#define HAVE_movqi 1
#define HAVE_movhi 1
#define HAVE_movsi 1
#define HAVE_movdi 1
#define HAVE_movti 1
#define HAVE_movcc 1
#define HAVE_movsf 1
#define HAVE_movdf 1
#define HAVE_reload_si_store 1
#define HAVE_reload_di_store (TARGET_POWERPC64)
#define HAVE_reload_si_load 1
#define HAVE_reload_di_load (TARGET_POWERPC64)
#define HAVE_movtf (!TARGET_IEEEQUAD && TARGET_LONG_DOUBLE_128)
#define HAVE_extenddftf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_extenddftf2_fprs (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT  \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_extendsftf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_trunctfdf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_trunctfsf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_floatsitf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_fix_trunctfsi2 (!TARGET_IEEEQUAD && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) && TARGET_LONG_DOUBLE_128)
#define HAVE_fix_trunctfsi2_fprs (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_LONG_DOUBLE_128)
#define HAVE_negtf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_abstf2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_abstf2_internal (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT  \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_load_multiple (TARGET_STRING && !TARGET_POWERPC64)
#define HAVE_store_multiple (TARGET_STRING && !TARGET_POWERPC64)
#define HAVE_setmemsi 1
#define HAVE_movmemsi 1
#define HAVE_movmemsi_8reg (TARGET_STRING)
#define HAVE_movmemsi_6reg (TARGET_STRING)
#define HAVE_movmemsi_4reg (TARGET_STRING)
#define HAVE_movmemsi_2reg (TARGET_STRING && ! TARGET_POWERPC64)
#define HAVE_movmemsi_1reg (TARGET_STRING)
#define HAVE_allocate_stack 1
#define HAVE_save_stack_function 1
#define HAVE_restore_stack_function 1
#define HAVE_restore_stack_block 1
#define HAVE_save_stack_nonlocal 1
#define HAVE_restore_stack_nonlocal 1
#define HAVE_load_toc_v4_PIC_1 (TARGET_ELF && DEFAULT_ABI != ABI_AIX \
   && (flag_pic == 2 || (flag_pic && TARGET_SECURE_PLT)))
#define HAVE_load_toc_v4_PIC_1b (TARGET_ELF && DEFAULT_ABI != ABI_AIX && flag_pic == 2)
#define HAVE_builtin_setjmp_receiver ((DEFAULT_ABI == ABI_V4 && flag_pic == 1) \
   || (TARGET_TOC && TARGET_MINIMAL_TOC) \
   || (DEFAULT_ABI == ABI_DARWIN && flag_pic))
#define HAVE_call 1
#define HAVE_call_value 1
#define HAVE_untyped_call 1
#define HAVE_sibcall 1
#define HAVE_sibcall_value 1
#define HAVE_sibcall_epilogue 1
#define HAVE_probe_stack 1
#define HAVE_cbranchsi4 1
#define HAVE_cbranchdi4 (TARGET_POWERPC64)
#define HAVE_cbranchsf4 (TARGET_HARD_FLOAT  \
   && ((TARGET_FPRS && TARGET_SINGLE_FLOAT) || TARGET_E500_SINGLE))
#define HAVE_cbranchdf4 (TARGET_HARD_FLOAT  \
   && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_cbranchtf4 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_cbranchdd4 (TARGET_DFP)
#define HAVE_cbranchtd4 (TARGET_DFP)
#define HAVE_cstoresi4 1
#define HAVE_cstoredi4 (TARGET_POWERPC64)
#define HAVE_cstoresf4 (TARGET_HARD_FLOAT  \
   && ((TARGET_FPRS && TARGET_SINGLE_FLOAT) || TARGET_E500_SINGLE))
#define HAVE_cstoredf4 (TARGET_HARD_FLOAT  \
   && ((TARGET_FPRS && TARGET_DOUBLE_FLOAT) || TARGET_E500_DOUBLE))
#define HAVE_cstoretf4 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT \
   && (TARGET_FPRS || TARGET_E500_DOUBLE) \
   && TARGET_LONG_DOUBLE_128)
#define HAVE_cstoredd4 (TARGET_DFP)
#define HAVE_cstoretd4 (TARGET_DFP)
#define HAVE_stack_protect_set 1
#define HAVE_stack_protect_test 1
#define HAVE_indirect_jump 1
#define HAVE_tablejump 1
#define HAVE_tablejumpsi (TARGET_32BIT)
#define HAVE_tablejumpdi (TARGET_64BIT)
#define HAVE_doloop_end 1
#define HAVE_ctrsi (TARGET_32BIT)
#define HAVE_ctrdi (TARGET_64BIT)
#define HAVE_ctrapsi4 1
#define HAVE_ctrapdi4 (TARGET_POWERPC64)
#define HAVE_prologue 1
#define HAVE_epilogue 1
#define HAVE_movsi_to_cr_one 1
#define HAVE_eh_return 1
#define HAVE_fmasf4 (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_fmadf4 ((TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT) \
       || VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_fmav4sf4 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_fmav2df4 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_fmssf4 ((!VECTOR_UNIT_ALTIVEC_P (SFmode)) && (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT))
#define HAVE_fmsdf4 ((!VECTOR_UNIT_ALTIVEC_P (DFmode)) && ((TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT) \
       || VECTOR_UNIT_VSX_P (DFmode)))
#define HAVE_fmsv4sf4 ((!VECTOR_UNIT_ALTIVEC_P (V4SFmode)) && (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode)))
#define HAVE_fmsv2df4 ((!VECTOR_UNIT_ALTIVEC_P (V2DFmode)) && (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode)))
#define HAVE_fnmasf4 ((!HONOR_SIGNED_ZEROS (SFmode)) && (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT))
#define HAVE_fnmadf4 ((!HONOR_SIGNED_ZEROS (DFmode)) && ((TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT) \
       || VECTOR_UNIT_VSX_P (DFmode)))
#define HAVE_fnmav4sf4 ((!HONOR_SIGNED_ZEROS (V4SFmode)) && (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode)))
#define HAVE_fnmav2df4 ((!HONOR_SIGNED_ZEROS (V2DFmode)) && (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode)))
#define HAVE_fnmssf4 ((!HONOR_SIGNED_ZEROS (SFmode) && !VECTOR_UNIT_ALTIVEC_P (SFmode)) && (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT))
#define HAVE_fnmsdf4 ((!HONOR_SIGNED_ZEROS (DFmode) && !VECTOR_UNIT_ALTIVEC_P (DFmode)) && ((TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT) \
       || VECTOR_UNIT_VSX_P (DFmode)))
#define HAVE_fnmsv4sf4 ((!HONOR_SIGNED_ZEROS (V4SFmode) && !VECTOR_UNIT_ALTIVEC_P (V4SFmode)) && (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode)))
#define HAVE_fnmsv2df4 ((!HONOR_SIGNED_ZEROS (V2DFmode) && !VECTOR_UNIT_ALTIVEC_P (V2DFmode)) && (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode)))
#define HAVE_nfmasf4 ((!VECTOR_UNIT_ALTIVEC_P (SFmode)) && (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT))
#define HAVE_nfmadf4 ((!VECTOR_UNIT_ALTIVEC_P (DFmode)) && ((TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT) \
       || VECTOR_UNIT_VSX_P (DFmode)))
#define HAVE_nfmav4sf4 ((!VECTOR_UNIT_ALTIVEC_P (V4SFmode)) && (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode)))
#define HAVE_nfmav2df4 ((!VECTOR_UNIT_ALTIVEC_P (V2DFmode)) && (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode)))
#define HAVE_nfmssf4 (TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_SINGLE_FLOAT)
#define HAVE_nfmsdf4 ((TARGET_HARD_FLOAT && TARGET_FPRS && TARGET_DOUBLE_FLOAT) \
       || VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_nfmsv4sf4 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_nfmsv2df4 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_rs6000_get_timebase 1
#define HAVE_mem_thread_fence 1
#define HAVE_hwsync 1
#define HAVE_lwsync 1
#define HAVE_atomic_loadqi 1
#define HAVE_atomic_loadhi 1
#define HAVE_atomic_loadsi 1
#define HAVE_atomic_loaddi (TARGET_POWERPC64)
#define HAVE_atomic_storeqi 1
#define HAVE_atomic_storehi 1
#define HAVE_atomic_storesi 1
#define HAVE_atomic_storedi (TARGET_POWERPC64)
#define HAVE_atomic_compare_and_swapqi 1
#define HAVE_atomic_compare_and_swaphi 1
#define HAVE_atomic_compare_and_swapsi 1
#define HAVE_atomic_compare_and_swapdi (TARGET_POWERPC64)
#define HAVE_atomic_exchangeqi 1
#define HAVE_atomic_exchangehi 1
#define HAVE_atomic_exchangesi 1
#define HAVE_atomic_exchangedi (TARGET_POWERPC64)
#define HAVE_atomic_addqi 1
#define HAVE_atomic_subqi 1
#define HAVE_atomic_orqi 1
#define HAVE_atomic_xorqi 1
#define HAVE_atomic_andqi 1
#define HAVE_atomic_addhi 1
#define HAVE_atomic_subhi 1
#define HAVE_atomic_orhi 1
#define HAVE_atomic_xorhi 1
#define HAVE_atomic_andhi 1
#define HAVE_atomic_addsi 1
#define HAVE_atomic_subsi 1
#define HAVE_atomic_orsi 1
#define HAVE_atomic_xorsi 1
#define HAVE_atomic_andsi 1
#define HAVE_atomic_adddi (TARGET_POWERPC64)
#define HAVE_atomic_subdi (TARGET_POWERPC64)
#define HAVE_atomic_ordi (TARGET_POWERPC64)
#define HAVE_atomic_xordi (TARGET_POWERPC64)
#define HAVE_atomic_anddi (TARGET_POWERPC64)
#define HAVE_atomic_nandqi 1
#define HAVE_atomic_nandhi 1
#define HAVE_atomic_nandsi 1
#define HAVE_atomic_nanddi (TARGET_POWERPC64)
#define HAVE_atomic_fetch_addqi 1
#define HAVE_atomic_fetch_subqi 1
#define HAVE_atomic_fetch_orqi 1
#define HAVE_atomic_fetch_xorqi 1
#define HAVE_atomic_fetch_andqi 1
#define HAVE_atomic_fetch_addhi 1
#define HAVE_atomic_fetch_subhi 1
#define HAVE_atomic_fetch_orhi 1
#define HAVE_atomic_fetch_xorhi 1
#define HAVE_atomic_fetch_andhi 1
#define HAVE_atomic_fetch_addsi 1
#define HAVE_atomic_fetch_subsi 1
#define HAVE_atomic_fetch_orsi 1
#define HAVE_atomic_fetch_xorsi 1
#define HAVE_atomic_fetch_andsi 1
#define HAVE_atomic_fetch_adddi (TARGET_POWERPC64)
#define HAVE_atomic_fetch_subdi (TARGET_POWERPC64)
#define HAVE_atomic_fetch_ordi (TARGET_POWERPC64)
#define HAVE_atomic_fetch_xordi (TARGET_POWERPC64)
#define HAVE_atomic_fetch_anddi (TARGET_POWERPC64)
#define HAVE_atomic_fetch_nandqi 1
#define HAVE_atomic_fetch_nandhi 1
#define HAVE_atomic_fetch_nandsi 1
#define HAVE_atomic_fetch_nanddi (TARGET_POWERPC64)
#define HAVE_atomic_add_fetchqi 1
#define HAVE_atomic_sub_fetchqi 1
#define HAVE_atomic_or_fetchqi 1
#define HAVE_atomic_xor_fetchqi 1
#define HAVE_atomic_and_fetchqi 1
#define HAVE_atomic_add_fetchhi 1
#define HAVE_atomic_sub_fetchhi 1
#define HAVE_atomic_or_fetchhi 1
#define HAVE_atomic_xor_fetchhi 1
#define HAVE_atomic_and_fetchhi 1
#define HAVE_atomic_add_fetchsi 1
#define HAVE_atomic_sub_fetchsi 1
#define HAVE_atomic_or_fetchsi 1
#define HAVE_atomic_xor_fetchsi 1
#define HAVE_atomic_and_fetchsi 1
#define HAVE_atomic_add_fetchdi (TARGET_POWERPC64)
#define HAVE_atomic_sub_fetchdi (TARGET_POWERPC64)
#define HAVE_atomic_or_fetchdi (TARGET_POWERPC64)
#define HAVE_atomic_xor_fetchdi (TARGET_POWERPC64)
#define HAVE_atomic_and_fetchdi (TARGET_POWERPC64)
#define HAVE_atomic_nand_fetchqi 1
#define HAVE_atomic_nand_fetchhi 1
#define HAVE_atomic_nand_fetchsi 1
#define HAVE_atomic_nand_fetchdi (TARGET_POWERPC64)
#define HAVE_movv16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_movv8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_movv4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_movv2di (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_movv4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_movv2df (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_load_v16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_load_v8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_load_v4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_load_v2di (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vector_load_v4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_load_v2df (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_store_v16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_store_v8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_store_v4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_store_v2di (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vector_store_v4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_store_v2df (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_altivec_load_v16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_altivec_load_v8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_altivec_load_v4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_altivec_load_v2di (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vector_altivec_load_v4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_altivec_load_v2df (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_altivec_store_v16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_altivec_store_v8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_altivec_store_v4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_altivec_store_v2di (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vector_altivec_store_v4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_altivec_store_v2df (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_reload_v16qi_si_store (TARGET_32BIT)
#define HAVE_reload_v16qi_di_store (TARGET_64BIT)
#define HAVE_reload_v8hi_si_store (TARGET_32BIT)
#define HAVE_reload_v8hi_di_store (TARGET_64BIT)
#define HAVE_reload_v4si_si_store (TARGET_32BIT)
#define HAVE_reload_v4si_di_store (TARGET_64BIT)
#define HAVE_reload_v2di_si_store (TARGET_32BIT)
#define HAVE_reload_v2di_di_store (TARGET_64BIT)
#define HAVE_reload_v4sf_si_store (TARGET_32BIT)
#define HAVE_reload_v4sf_di_store (TARGET_64BIT)
#define HAVE_reload_v2df_si_store (TARGET_32BIT)
#define HAVE_reload_v2df_di_store (TARGET_64BIT)
#define HAVE_reload_df_si_store (TARGET_32BIT)
#define HAVE_reload_df_di_store (TARGET_64BIT)
#define HAVE_reload_ti_si_store (TARGET_32BIT)
#define HAVE_reload_ti_di_store (TARGET_64BIT)
#define HAVE_reload_v16qi_si_load (TARGET_32BIT)
#define HAVE_reload_v16qi_di_load (TARGET_64BIT)
#define HAVE_reload_v8hi_si_load (TARGET_32BIT)
#define HAVE_reload_v8hi_di_load (TARGET_64BIT)
#define HAVE_reload_v4si_si_load (TARGET_32BIT)
#define HAVE_reload_v4si_di_load (TARGET_64BIT)
#define HAVE_reload_v2di_si_load (TARGET_32BIT)
#define HAVE_reload_v2di_di_load (TARGET_64BIT)
#define HAVE_reload_v4sf_si_load (TARGET_32BIT)
#define HAVE_reload_v4sf_di_load (TARGET_64BIT)
#define HAVE_reload_v2df_si_load (TARGET_32BIT)
#define HAVE_reload_v2df_di_load (TARGET_64BIT)
#define HAVE_reload_df_si_load (TARGET_32BIT)
#define HAVE_reload_df_di_load (TARGET_64BIT)
#define HAVE_reload_ti_si_load (TARGET_32BIT)
#define HAVE_reload_ti_di_load (TARGET_64BIT)
#define HAVE_addv4sf3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_addv2df3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_subv4sf3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_subv2df3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_mulv4sf3 (VECTOR_UNIT_VSX_P (V4SFmode) || VECTOR_UNIT_ALTIVEC_P (V4SFmode))
#define HAVE_mulv2df3 (VECTOR_UNIT_VSX_P (V2DFmode) || VECTOR_UNIT_ALTIVEC_P (V2DFmode))
#define HAVE_divv4sf3 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_divv2df3 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_negv4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_negv2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_absv4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_absv2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_sminv4sf3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_sminv2df3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_smaxv4sf3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_smaxv2df3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_sqrtv4sf2 (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_sqrtv2df2 (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_rsqrtev4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_rsqrtev2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_rev4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_rev2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_ftruncv4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_ftruncv2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_ceilv4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_ceilv2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_floorv4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_floorv2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_btruncv4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_btruncv2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_copysignv4sf3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_copysignv2df3 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vcondv4sfv4sf (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vcondv2dfv2df (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vcondv16qiv16qi (VECTOR_UNIT_ALTIVEC_P (V16QImode))
#define HAVE_vcondv8hiv8hi (VECTOR_UNIT_ALTIVEC_P (V8HImode))
#define HAVE_vcondv4siv4si (VECTOR_UNIT_ALTIVEC_P (V4SImode))
#define HAVE_vcondv4sfv4si (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode) \
   && VECTOR_UNIT_ALTIVEC_P (V4SImode))
#define HAVE_vcondv4siv4sf (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode) \
   && VECTOR_UNIT_ALTIVEC_P (V4SImode))
#define HAVE_vconduv16qiv16qi (VECTOR_UNIT_ALTIVEC_P (V16QImode))
#define HAVE_vconduv8hiv8hi (VECTOR_UNIT_ALTIVEC_P (V8HImode))
#define HAVE_vconduv4siv4si (VECTOR_UNIT_ALTIVEC_P (V4SImode))
#define HAVE_vconduv4sfv4si (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode) \
   && VECTOR_UNIT_ALTIVEC_P (V4SImode))
#define HAVE_vector_eqv16qi (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_eqv8hi (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_eqv4si (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_eqv4sf (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_eqv2df (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_gtv16qi (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_gtv8hi (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_gtv4si (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_gtv4sf (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_gtv2df (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_gev16qi (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_gev8hi (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_gev4si (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_gev4sf (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_gev2df (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_gtuv16qi (VECTOR_UNIT_ALTIVEC_P (V16QImode))
#define HAVE_vector_gtuv8hi (VECTOR_UNIT_ALTIVEC_P (V8HImode))
#define HAVE_vector_gtuv4si (VECTOR_UNIT_ALTIVEC_P (V4SImode))
#define HAVE_vector_geuv16qi (VECTOR_UNIT_ALTIVEC_P (V16QImode))
#define HAVE_vector_geuv8hi (VECTOR_UNIT_ALTIVEC_P (V8HImode))
#define HAVE_vector_geuv4si (VECTOR_UNIT_ALTIVEC_P (V4SImode))
#define HAVE_vector_select_v16qi (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_select_v8hi (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_select_v4si (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_select_v2di (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vector_select_v4sf (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_select_v2df (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_select_ti (VECTOR_UNIT_ALTIVEC_OR_VSX_P (TImode))
#define HAVE_vector_select_v16qi_uns (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_select_v8hi_uns (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_select_v4si_uns (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_select_v2di_uns (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vector_select_v4sf_uns (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_select_v2df_uns (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_select_ti_uns (VECTOR_UNIT_ALTIVEC_OR_VSX_P (TImode))
#define HAVE_vector_eq_v16qi_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_eq_v8hi_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_eq_v4si_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_eq_v4sf_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_eq_v2df_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_gt_v16qi_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_gt_v8hi_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_gt_v4si_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vector_gt_v4sf_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_gt_v2df_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_ge_v4sf_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vector_ge_v2df_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vector_gtu_v16qi_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vector_gtu_v8hi_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vector_gtu_v4si_p (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_cr6_test_for_zero (TARGET_ALTIVEC || TARGET_VSX)
#define HAVE_cr6_test_for_zero_reverse (TARGET_ALTIVEC || TARGET_VSX)
#define HAVE_cr6_test_for_lt (TARGET_ALTIVEC || TARGET_VSX)
#define HAVE_cr6_test_for_lt_reverse (TARGET_ALTIVEC || TARGET_VSX)
#define HAVE_xorv16qi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_xorv8hi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_xorv4si3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_xorv2di3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_xorv4sf3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_xorv2df3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_xorti3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (TImode))
#define HAVE_iorv16qi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_iorv8hi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_iorv4si3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_iorv2di3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_iorv4sf3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_iorv2df3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_iorti3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (TImode))
#define HAVE_andv16qi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_andv8hi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_andv4si3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_andv2di3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_andv4sf3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_andv2df3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_andti3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (TImode))
#define HAVE_one_cmplv16qi2 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_one_cmplv8hi2 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_one_cmplv4si2 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_one_cmplv2di2 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_one_cmplv4sf2 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_one_cmplv2df2 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_one_cmplti2 (VECTOR_MEM_ALTIVEC_OR_VSX_P (TImode))
#define HAVE_norv16qi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_norv8hi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_norv4si3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_norv2di3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_norv4sf3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_norv2df3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_norti3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (TImode))
#define HAVE_andcv16qi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_andcv8hi3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_andcv4si3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_andcv2di3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_andcv4sf3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_andcv2df3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_andcti3 (VECTOR_MEM_ALTIVEC_OR_VSX_P (TImode))
#define HAVE_floatv4siv4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_floatv2div2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_floatunsv4siv4sf2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_floatunsv2div2df2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_fix_truncv4sfv4si2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_fix_truncv2dfv2di2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_fixuns_truncv4sfv4si2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_fixuns_truncv2dfv2di2 (VECTOR_UNIT_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vec_initv16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vec_initv8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vec_initv4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vec_initv2di (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vec_initv4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vec_initv2df (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vec_setv16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vec_setv8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vec_setv4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vec_setv2di (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vec_setv4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vec_setv2df (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vec_extractv16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vec_extractv8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vec_extractv4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vec_extractv2di (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DImode))
#define HAVE_vec_extractv4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vec_extractv2df (VECTOR_MEM_ALTIVEC_OR_VSX_P (V2DFmode))
#define HAVE_vec_pack_trunc_v2df (VECTOR_UNIT_VSX_P (V2DFmode) && TARGET_ALTIVEC)
#define HAVE_vec_pack_sfix_trunc_v2df (VECTOR_UNIT_VSX_P (V2DFmode) && TARGET_ALTIVEC)
#define HAVE_vec_pack_ufix_trunc_v2df (VECTOR_UNIT_VSX_P (V2DFmode) && TARGET_ALTIVEC)
#define HAVE_vec_unpacks_hi_v4sf (VECTOR_UNIT_VSX_P (V2DFmode) && VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vec_unpacks_lo_v4sf (VECTOR_UNIT_VSX_P (V2DFmode) && VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_vec_unpacks_float_hi_v4si (VECTOR_UNIT_VSX_P (V2DFmode) && VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vec_unpacks_float_lo_v4si (VECTOR_UNIT_VSX_P (V2DFmode) && VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vec_unpacku_float_hi_v4si (VECTOR_UNIT_VSX_P (V2DFmode) && VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vec_unpacku_float_lo_v4si (VECTOR_UNIT_VSX_P (V2DFmode) && VECTOR_UNIT_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vec_realign_load_v16qi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V16QImode))
#define HAVE_vec_realign_load_v8hi (VECTOR_MEM_ALTIVEC_OR_VSX_P (V8HImode))
#define HAVE_vec_realign_load_v4si (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SImode))
#define HAVE_vec_realign_load_v4sf (VECTOR_MEM_ALTIVEC_OR_VSX_P (V4SFmode))
#define HAVE_movmisalignv4si (VECTOR_MEM_VSX_P (V4SImode) && TARGET_ALLOW_MOVMISALIGN)
#define HAVE_movmisalignv4sf (VECTOR_MEM_VSX_P (V4SFmode) && TARGET_ALLOW_MOVMISALIGN)
#define HAVE_movmisalignv2di (VECTOR_MEM_VSX_P (V2DImode) && TARGET_ALLOW_MOVMISALIGN)
#define HAVE_movmisalignv2df (VECTOR_MEM_VSX_P (V2DFmode) && TARGET_ALLOW_MOVMISALIGN)
#define HAVE_vec_shl_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_shl_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_shl_v4si (TARGET_ALTIVEC)
#define HAVE_vec_shl_v2di (TARGET_ALTIVEC)
#define HAVE_vec_shl_v4sf (TARGET_ALTIVEC)
#define HAVE_vec_shl_v2df (TARGET_ALTIVEC)
#define HAVE_vec_shl_ti (TARGET_ALTIVEC)
#define HAVE_vec_shr_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_shr_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_shr_v4si (TARGET_ALTIVEC)
#define HAVE_vec_shr_v2di (TARGET_ALTIVEC)
#define HAVE_vec_shr_v4sf (TARGET_ALTIVEC)
#define HAVE_vec_shr_v2df (TARGET_ALTIVEC)
#define HAVE_vec_shr_ti (TARGET_ALTIVEC)
#define HAVE_vrotlv16qi3 (TARGET_ALTIVEC)
#define HAVE_vrotlv8hi3 (TARGET_ALTIVEC)
#define HAVE_vrotlv4si3 (TARGET_ALTIVEC)
#define HAVE_vashlv16qi3 (TARGET_ALTIVEC)
#define HAVE_vashlv8hi3 (TARGET_ALTIVEC)
#define HAVE_vashlv4si3 (TARGET_ALTIVEC)
#define HAVE_vlshrv16qi3 (TARGET_ALTIVEC)
#define HAVE_vlshrv8hi3 (TARGET_ALTIVEC)
#define HAVE_vlshrv4si3 (TARGET_ALTIVEC)
#define HAVE_vashrv16qi3 (TARGET_ALTIVEC)
#define HAVE_vashrv8hi3 (TARGET_ALTIVEC)
#define HAVE_vashrv4si3 (TARGET_ALTIVEC)
#define HAVE_reduc_splus_v2df (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_reduc_smin_v2df (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_reduc_smax_v2df (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_reduc_splus_v4sf (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_reduc_smin_v4sf (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_reduc_smax_v4sf (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_absv2sf2 (TARGET_PAIRED_FLOAT || TARGET_SPE)
#define HAVE_negv2sf2 (TARGET_PAIRED_FLOAT || TARGET_SPE)
#define HAVE_addv2sf3 (TARGET_PAIRED_FLOAT || TARGET_SPE)
#define HAVE_subv2sf3 (TARGET_PAIRED_FLOAT || TARGET_SPE)
#define HAVE_mulv2sf3 (TARGET_PAIRED_FLOAT || TARGET_SPE)
#define HAVE_divv2sf3 (TARGET_PAIRED_FLOAT || TARGET_SPE)
#define HAVE_vsx_load_v16qi (VECTOR_MEM_VSX_P (V16QImode))
#define HAVE_vsx_load_v8hi (VECTOR_MEM_VSX_P (V8HImode))
#define HAVE_vsx_load_v4si (VECTOR_MEM_VSX_P (V4SImode))
#define HAVE_vsx_load_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_load_v4sf (VECTOR_MEM_VSX_P (V4SFmode))
#define HAVE_vsx_load_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_store_v16qi (VECTOR_MEM_VSX_P (V16QImode))
#define HAVE_vsx_store_v8hi (VECTOR_MEM_VSX_P (V8HImode))
#define HAVE_vsx_store_v4si (VECTOR_MEM_VSX_P (V4SImode))
#define HAVE_vsx_store_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_store_v4sf (VECTOR_MEM_VSX_P (V4SFmode))
#define HAVE_vsx_store_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_tdivdf3_fg (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_tdivv4sf3_fg (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_tdivv2df3_fg (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_tdivdf3_fe (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_tdivv4sf3_fe (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_tdivv2df3_fe (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_tsqrtdf2_fg (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_tsqrtv4sf2_fg (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_tsqrtv2df2_fg (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_tsqrtdf2_fe (VECTOR_UNIT_VSX_P (DFmode))
#define HAVE_vsx_tsqrtv4sf2_fe (VECTOR_UNIT_VSX_P (V4SFmode))
#define HAVE_vsx_tsqrtv2df2_fe (VECTOR_UNIT_VSX_P (V2DFmode))
#define HAVE_vsx_xxpermdi_v16qi (VECTOR_MEM_VSX_P (V16QImode))
#define HAVE_vsx_xxpermdi_v8hi (VECTOR_MEM_VSX_P (V8HImode))
#define HAVE_vsx_xxpermdi_v4si (VECTOR_MEM_VSX_P (V4SImode))
#define HAVE_vsx_xxpermdi_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_xxpermdi_v4sf (VECTOR_MEM_VSX_P (V4SFmode))
#define HAVE_vsx_xxpermdi_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_xxpermdi_ti (VECTOR_MEM_VSX_P (TImode))
#define HAVE_vec_perm_constv2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vec_perm_constv2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_mergel_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_mergel_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_vsx_mergeh_v2df (VECTOR_MEM_VSX_P (V2DFmode))
#define HAVE_vsx_mergeh_v2di (VECTOR_MEM_VSX_P (V2DImode))
#define HAVE_altivec_mulv4sf3 (VECTOR_UNIT_ALTIVEC_P (V4SFmode))
#define HAVE_mulv4si3 (TARGET_ALTIVEC)
#define HAVE_mulv8hi3 (TARGET_ALTIVEC)
#define HAVE_vec_permv16qi (TARGET_ALTIVEC)
#define HAVE_vec_perm_constv16qi (TARGET_ALTIVEC)
#define HAVE_altivec_copysign_v4sf3 (VECTOR_UNIT_ALTIVEC_P (V4SFmode))
#define HAVE_build_vector_mask_for_load (TARGET_ALTIVEC)
#define HAVE_absv4si2 (TARGET_ALTIVEC)
#define HAVE_absv8hi2 (TARGET_ALTIVEC)
#define HAVE_absv16qi2 (TARGET_ALTIVEC)
#define HAVE_altivec_absv4sf2 (TARGET_ALTIVEC)
#define HAVE_altivec_abss_v4si (TARGET_ALTIVEC)
#define HAVE_altivec_abss_v8hi (TARGET_ALTIVEC)
#define HAVE_altivec_abss_v16qi (TARGET_ALTIVEC)
#define HAVE_reduc_splus_v8hi (TARGET_ALTIVEC)
#define HAVE_reduc_splus_v16qi (TARGET_ALTIVEC)
#define HAVE_reduc_uplus_v16qi (TARGET_ALTIVEC)
#define HAVE_negv4si2 (TARGET_ALTIVEC)
#define HAVE_negv8hi2 (TARGET_ALTIVEC)
#define HAVE_negv16qi2 (TARGET_ALTIVEC)
#define HAVE_udot_prodv8hi (TARGET_ALTIVEC)
#define HAVE_udot_prodv16qi (TARGET_ALTIVEC)
#define HAVE_sdot_prodv8hi (TARGET_ALTIVEC)
#define HAVE_widen_usumv8hi3 (TARGET_ALTIVEC)
#define HAVE_widen_usumv16qi3 (TARGET_ALTIVEC)
#define HAVE_widen_ssumv16qi3 (TARGET_ALTIVEC)
#define HAVE_widen_ssumv8hi3 (TARGET_ALTIVEC)
#define HAVE_vec_unpacks_hi_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_unpacks_hi_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_unpacks_lo_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_unpacks_lo_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_unpacku_hi_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_unpacku_hi_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_unpacku_lo_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_unpacku_lo_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_widen_umult_hi_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_widen_umult_lo_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_widen_smult_hi_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_widen_smult_lo_v16qi (TARGET_ALTIVEC)
#define HAVE_vec_widen_umult_hi_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_widen_umult_lo_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_widen_smult_hi_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_widen_smult_lo_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_pack_trunc_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_pack_trunc_v4si (TARGET_ALTIVEC)
#define HAVE_altivec_negv4sf2 (TARGET_ALTIVEC)
#define HAVE_vec_unpacks_float_hi_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_unpacks_float_lo_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_unpacku_float_hi_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_unpacku_float_lo_v8hi (TARGET_ALTIVEC)
#define HAVE_vec_perm_constv2si (TARGET_SPE)
#define HAVE_spe_fix_trunctfsi2 (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128)
#define HAVE_spe_abstf2_cmp (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128)
#define HAVE_spe_abstf2_tst (!TARGET_IEEEQUAD \
   && TARGET_HARD_FLOAT && TARGET_E500_DOUBLE && TARGET_LONG_DOUBLE_128)
#define HAVE_movv2si (TARGET_SPE)
#define HAVE_movv1di (TARGET_SPE)
#define HAVE_movv4hi (TARGET_SPE)
#define HAVE_movv2sf (TARGET_SPE || TARGET_PAIRED_FLOAT)
#define HAVE_movsd (TARGET_HARD_FLOAT && TARGET_FPRS)
#define HAVE_extendsdtd2 (TARGET_DFP)
#define HAVE_negdd2 (TARGET_HARD_FLOAT && TARGET_FPRS)
#define HAVE_absdd2 (TARGET_HARD_FLOAT && TARGET_FPRS)
#define HAVE_movdd 1
#define HAVE_negtd2 (TARGET_HARD_FLOAT && TARGET_FPRS)
#define HAVE_abstd2 (TARGET_HARD_FLOAT && TARGET_FPRS)
#define HAVE_movtd (TARGET_HARD_FLOAT && TARGET_FPRS)
static inline rtx gen_adddi3_high                        (rtx, rtx, rtx);
static inline rtx
gen_adddi3_high(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movdf_low_si                       (rtx, rtx, rtx);
static inline rtx
gen_movdf_low_si(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movdf_low_di                       (rtx, rtx, rtx);
static inline rtx
gen_movdf_low_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movdf_low_st_si                    (rtx, rtx, rtx);
static inline rtx
gen_movdf_low_st_si(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movdf_low_st_di                    (rtx, rtx, rtx);
static inline rtx
gen_movdf_low_st_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movsf_low_si                       (rtx, rtx, rtx);
static inline rtx
gen_movsf_low_si(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movsf_low_di                       (rtx, rtx, rtx);
static inline rtx
gen_movsf_low_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movsf_low_st_si                    (rtx, rtx, rtx);
static inline rtx
gen_movsf_low_st_si(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movsf_low_st_di                    (rtx, rtx, rtx);
static inline rtx
gen_movsf_low_st_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movdi_low                          (rtx, rtx, rtx);
static inline rtx
gen_movdi_low(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movsi_low_st                       (rtx, rtx, rtx);
static inline rtx
gen_movsi_low_st(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_movdi_low_st                       (rtx, rtx, rtx);
static inline rtx
gen_movdi_low_st(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_macho_high_si                      (rtx, rtx);
static inline rtx
gen_macho_high_si(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_macho_high_di                      (rtx, rtx);
static inline rtx
gen_macho_high_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_macho_low_si                       (rtx, rtx, rtx);
static inline rtx
gen_macho_low_si(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_macho_low_di                       (rtx, rtx, rtx);
static inline rtx
gen_macho_low_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_load_macho_picbase_si              (rtx);
extern rtx        gen_load_macho_picbase_di              (rtx);
extern rtx        gen_macho_correct_pic_si               (rtx, rtx, rtx, rtx);
extern rtx        gen_macho_correct_pic_di               (rtx, rtx, rtx, rtx);
extern rtx        gen_extendqidi2                        (rtx, rtx);
extern rtx        gen_extendqisi2                        (rtx, rtx);
extern rtx        gen_extendqihi2                        (rtx, rtx);
extern rtx        gen_dlmzb                              (rtx, rtx, rtx, rtx);
static inline rtx gen_addsi3_high                        (rtx, rtx, rtx);
static inline rtx
gen_addsi3_high(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_one_cmplsi2                        (rtx, rtx);
extern rtx        gen_one_cmpldi2                        (rtx, rtx);
extern rtx        gen_clzsi2                             (rtx, rtx);
extern rtx        gen_clzdi2                             (rtx, rtx);
extern rtx        gen_popcntbsi2                         (rtx, rtx);
extern rtx        gen_popcntbdi2                         (rtx, rtx);
extern rtx        gen_popcntdsi2                         (rtx, rtx);
extern rtx        gen_popcntddi2                         (rtx, rtx);
extern rtx        gen_paritysi2_cmpb                     (rtx, rtx);
extern rtx        gen_paritydi2_cmpb                     (rtx, rtx);
extern rtx        gen_bswaphi2_internal                  (rtx, rtx);
extern rtx        gen_bswapdi2_32bit                     (rtx, rtx);
extern rtx        gen_mulsi3                             (rtx, rtx, rtx);
extern rtx        gen_udivsi3                            (rtx, rtx, rtx);
extern rtx        gen_udivdi3                            (rtx, rtx, rtx);
extern rtx        gen_andsi3_mc                          (rtx, rtx, rtx);
extern rtx        gen_andsi3_nomc                        (rtx, rtx, rtx);
extern rtx        gen_andsi3_internal0_nomc              (rtx, rtx, rtx);
extern rtx        gen_insvsi_internal                    (rtx, rtx, rtx, rtx);
extern rtx        gen_insvdi_internal                    (rtx, rtx, rtx, rtx);
extern rtx        gen_extzvsi_internal                   (rtx, rtx, rtx, rtx);
extern rtx        gen_extzvdi_internal                   (rtx, rtx, rtx, rtx);
extern rtx        gen_rotlsi3                            (rtx, rtx, rtx);
extern rtx        gen_ashlsi3                            (rtx, rtx, rtx);
extern rtx        gen_rlwinm                             (rtx, rtx, rtx, rtx);
extern rtx        gen_lshrsi3                            (rtx, rtx, rtx);
extern rtx        gen_ashrsi3                            (rtx, rtx, rtx);
extern rtx        gen_fres                               (rtx, rtx);
extern rtx        gen_copysignsf3_fcpsgn                 (rtx, rtx, rtx);
extern rtx        gen_copysigndf3_fcpsgn                 (rtx, rtx, rtx);
extern rtx        gen_isel_signed_si                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_isel_signed_di                     (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_isel_unsigned_si                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_isel_unsigned_di                   (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_lfiwax                             (rtx, rtx);
extern rtx        gen_floatsisf2_lfiwax                  (rtx, rtx);
extern rtx        gen_floatsidf2_lfiwax                  (rtx, rtx);
extern rtx        gen_floatsisf2_lfiwax_mem              (rtx, rtx);
extern rtx        gen_floatsidf2_lfiwax_mem              (rtx, rtx);
extern rtx        gen_lfiwzx                             (rtx, rtx);
extern rtx        gen_floatunssisf2_lfiwzx               (rtx, rtx);
extern rtx        gen_floatunssidf2_lfiwzx               (rtx, rtx);
extern rtx        gen_floatunssisf2_lfiwzx_mem           (rtx, rtx);
extern rtx        gen_floatunssidf2_lfiwzx_mem           (rtx, rtx);
extern rtx        gen_fix_truncsfsi2_stfiwx              (rtx, rtx);
extern rtx        gen_fix_truncdfsi2_stfiwx              (rtx, rtx);
extern rtx        gen_fix_truncsfsi2_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_fix_truncdfsi2_internal            (rtx, rtx, rtx, rtx);
extern rtx        gen_fixuns_truncsfsi2_stfiwx           (rtx, rtx);
extern rtx        gen_fixuns_truncdfsi2_stfiwx           (rtx, rtx);
extern rtx        gen_fctiwz_sf                          (rtx, rtx);
extern rtx        gen_fctiwz_df                          (rtx, rtx);
extern rtx        gen_fctiwuz_sf                         (rtx, rtx);
extern rtx        gen_fctiwuz_df                         (rtx, rtx);
extern rtx        gen_lrintsfdi2                         (rtx, rtx);
extern rtx        gen_lrintdfdi2                         (rtx, rtx);
extern rtx        gen_roundsf2                           (rtx, rtx);
extern rtx        gen_rounddf2                           (rtx, rtx);
extern rtx        gen_stfiwx                             (rtx, rtx);
extern rtx        gen_floatdisf2_fcfids                  (rtx, rtx);
extern rtx        gen_floatdisf2_internal1               (rtx, rtx);
extern rtx        gen_floatunsdisf2_fcfidus              (rtx, rtx);
extern rtx        gen_mulsidi3                           (rtx, rtx, rtx);
extern rtx        gen_umulsidi3                          (rtx, rtx, rtx);
extern rtx        gen_smulsi3_highpart                   (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart                   (rtx, rtx, rtx);
extern rtx        gen_ashrdi3_no_power                   (rtx, rtx, rtx);
extern rtx        gen_muldi3                             (rtx, rtx, rtx);
extern rtx        gen_smuldi3_highpart                   (rtx, rtx, rtx);
extern rtx        gen_umuldi3_highpart                   (rtx, rtx, rtx);
extern rtx        gen_rotldi3                            (rtx, rtx, rtx);
extern rtx        gen_ashldi3_internal5                  (rtx, rtx, rtx, rtx);
extern rtx        gen_ashldi3_internal8                  (rtx, rtx, rtx, rtx);
extern rtx        gen_anddi3_mc                          (rtx, rtx, rtx);
extern rtx        gen_anddi3_nomc                        (rtx, rtx, rtx);
static inline rtx gen_movsi_low                          (rtx, rtx, rtx);
static inline rtx
gen_movsi_low(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_trunctfdf2_internal1               (rtx, rtx);
extern rtx        gen_trunctfdf2_internal2               (rtx, rtx);
extern rtx        gen_trunctfsf2_fprs                    (rtx, rtx);
extern rtx        gen_fix_trunc_helper                   (rtx, rtx, rtx);
extern rtx        gen_negtf2_internal                    (rtx, rtx);
extern rtx        gen_movdi_si_update                    (rtx, rtx, rtx, rtx);
extern rtx        gen_movdi_di_update                    (rtx, rtx, rtx, rtx);
extern rtx        gen_movdi_si_update_stack              (rtx, rtx, rtx, rtx);
extern rtx        gen_movdi_di_update_stack              (rtx, rtx, rtx, rtx);
extern rtx        gen_movsi_update                       (rtx, rtx, rtx, rtx);
extern rtx        gen_movsi_update_stack                 (rtx, rtx, rtx, rtx);
extern rtx        gen_tls_gd_aix32                       (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_tls_gd_aix64                       (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_tls_gd_sysvsi                      (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_tls_gd_sysvdi                      (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_tls_ld_aix32                       (rtx, rtx, rtx, rtx);
extern rtx        gen_tls_ld_aix64                       (rtx, rtx, rtx, rtx);
extern rtx        gen_tls_ld_sysvsi                      (rtx, rtx, rtx, rtx);
extern rtx        gen_tls_ld_sysvdi                      (rtx, rtx, rtx, rtx);
extern rtx        gen_tls_dtprel_32                      (rtx, rtx, rtx);
extern rtx        gen_tls_dtprel_64                      (rtx, rtx, rtx);
extern rtx        gen_tls_dtprel_ha_32                   (rtx, rtx, rtx);
extern rtx        gen_tls_dtprel_ha_64                   (rtx, rtx, rtx);
extern rtx        gen_tls_dtprel_lo_32                   (rtx, rtx, rtx);
extern rtx        gen_tls_dtprel_lo_64                   (rtx, rtx, rtx);
extern rtx        gen_tls_got_dtprel_32                  (rtx, rtx, rtx);
extern rtx        gen_tls_got_dtprel_64                  (rtx, rtx, rtx);
extern rtx        gen_tls_tprel_32                       (rtx, rtx, rtx);
extern rtx        gen_tls_tprel_64                       (rtx, rtx, rtx);
extern rtx        gen_tls_tprel_ha_32                    (rtx, rtx, rtx);
extern rtx        gen_tls_tprel_ha_64                    (rtx, rtx, rtx);
extern rtx        gen_tls_tprel_lo_32                    (rtx, rtx, rtx);
extern rtx        gen_tls_tprel_lo_64                    (rtx, rtx, rtx);
extern rtx        gen_tls_got_tprel_32                   (rtx, rtx, rtx);
extern rtx        gen_tls_got_tprel_64                   (rtx, rtx, rtx);
extern rtx        gen_tls_tls_32                         (rtx, rtx, rtx);
extern rtx        gen_tls_tls_64                         (rtx, rtx, rtx);
static inline rtx gen_tls_get_tpointer_internal          (void);
static inline rtx
gen_tls_get_tpointer_internal(void)
{
  return 0;
}
static inline rtx gen_tls_get_addr_internalsi            (void);
static inline rtx
gen_tls_get_addr_internalsi(void)
{
  return 0;
}
static inline rtx gen_tls_get_addr_internaldi            (void);
static inline rtx
gen_tls_get_addr_internaldi(void)
{
  return 0;
}
extern rtx        gen_load_toc_aix_si                    (rtx);
extern rtx        gen_load_toc_aix_di                    (rtx);
extern rtx        gen_load_toc_v4_pic_si                 (void);
extern rtx        gen_load_toc_v4_PIC_1_normal           (rtx);
static inline rtx gen_load_toc_v4_PIC_1_476              (rtx);
static inline rtx
gen_load_toc_v4_PIC_1_476(rtx ARG_UNUSED (a))
{
  return 0;
}
extern rtx        gen_load_toc_v4_PIC_1b_normal          (rtx, rtx);
static inline rtx gen_load_toc_v4_PIC_1b_476             (rtx, rtx);
static inline rtx
gen_load_toc_v4_PIC_1b_476(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
extern rtx        gen_load_toc_v4_PIC_2                  (rtx, rtx, rtx, rtx);
extern rtx        gen_load_toc_v4_PIC_3b                 (rtx, rtx, rtx, rtx);
extern rtx        gen_load_toc_v4_PIC_3c                 (rtx, rtx, rtx, rtx);
extern rtx        gen_elf_high                           (rtx, rtx);
extern rtx        gen_elf_low                            (rtx, rtx, rtx);
extern rtx        gen_call_indirect_aix32bit             (rtx, rtx, rtx, rtx);
extern rtx        gen_call_indirect_aix64bit             (rtx, rtx, rtx, rtx);
extern rtx        gen_call_indirect_aix32bit_nor11       (rtx, rtx, rtx, rtx);
extern rtx        gen_call_indirect_aix64bit_nor11       (rtx, rtx, rtx, rtx);
extern rtx        gen_call_value_indirect_aix32bit       (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_call_value_indirect_aix64bit       (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_call_value_indirect_aix32bit_nor11 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_call_value_indirect_aix64bit_nor11 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_blockage                           (void);
extern rtx        gen_probe_stack_si                     (rtx);
extern rtx        gen_probe_stack_di                     (rtx);
extern rtx        gen_probe_stack_rangesi                (rtx, rtx, rtx);
extern rtx        gen_probe_stack_rangedi                (rtx, rtx, rtx);
extern rtx        gen_stack_protect_setsi                (rtx, rtx);
extern rtx        gen_stack_protect_setdi                (rtx, rtx);
extern rtx        gen_stack_protect_testsi               (rtx, rtx, rtx);
extern rtx        gen_stack_protect_testdi               (rtx, rtx, rtx);
extern rtx        gen_move_from_CR_gt_bit                (rtx, rtx);
extern rtx        gen_move_from_CR_ov_bit                (rtx, rtx);
extern rtx        gen_jump                               (rtx);
extern rtx        gen_return                             (void);
extern rtx        gen_simple_return                      (void);
extern rtx        gen_nop                                (void);
extern rtx        gen_group_ending_nop                   (void);
extern rtx        gen_trap                               (void);
extern rtx        gen_movesi_from_cr                     (rtx);
extern rtx        gen_stack_tie                          (rtx);
extern rtx        gen_eh_set_lr_si                       (rtx);
extern rtx        gen_eh_set_lr_di                       (rtx);
extern rtx        gen_prefetch                           (rtx, rtx, rtx);
extern rtx        gen_bpermd_si                          (rtx, rtx, rtx);
extern rtx        gen_bpermd_di                          (rtx, rtx, rtx);
extern rtx        gen_rs6000_get_timebase_ppc32          (rtx);
extern rtx        gen_rs6000_mftb_si                     (rtx);
extern rtx        gen_rs6000_mftb_di                     (rtx);
extern rtx        gen_isync                              (void);
extern rtx        gen_loadsync_qi                        (rtx);
extern rtx        gen_loadsync_hi                        (rtx);
extern rtx        gen_loadsync_si                        (rtx);
extern rtx        gen_loadsync_di                        (rtx);
extern rtx        gen_load_lockedsi                      (rtx, rtx);
extern rtx        gen_load_lockeddi                      (rtx, rtx);
extern rtx        gen_store_conditionalsi                (rtx, rtx, rtx);
extern rtx        gen_store_conditionaldi                (rtx, rtx, rtx);
extern rtx        gen_vsx_fredf2                         (rtx, rtx);
extern rtx        gen_vsx_frev4sf2                       (rtx, rtx);
extern rtx        gen_vsx_frev2df2                       (rtx, rtx);
extern rtx        gen_vsx_nabsdf2                        (rtx, rtx);
extern rtx        gen_vsx_nabsv4sf2                      (rtx, rtx);
extern rtx        gen_vsx_nabsv2df2                      (rtx, rtx);
extern rtx        gen_vsx_smaxdf3                        (rtx, rtx, rtx);
extern rtx        gen_vsx_smaxv4sf3                      (rtx, rtx, rtx);
extern rtx        gen_vsx_smaxv2df3                      (rtx, rtx, rtx);
extern rtx        gen_vsx_eqv4sf                         (rtx, rtx, rtx);
extern rtx        gen_vsx_eqv2df                         (rtx, rtx, rtx);
extern rtx        gen_vsx_gtv4sf                         (rtx, rtx, rtx);
extern rtx        gen_vsx_gtv2df                         (rtx, rtx, rtx);
extern rtx        gen_vsx_copysigndf3                    (rtx, rtx, rtx);
extern rtx        gen_vsx_copysignv4sf3                  (rtx, rtx, rtx);
extern rtx        gen_vsx_copysignv2df3                  (rtx, rtx, rtx);
extern rtx        gen_vsx_floatdidf2                     (rtx, rtx);
extern rtx        gen_vsx_floatv4siv4sf2                 (rtx, rtx);
extern rtx        gen_vsx_floatv2div2df2                 (rtx, rtx);
extern rtx        gen_vsx_floatunsdidf2                  (rtx, rtx);
extern rtx        gen_vsx_floatunsv4siv4sf2              (rtx, rtx);
extern rtx        gen_vsx_floatunsv2div2df2              (rtx, rtx);
extern rtx        gen_vsx_fix_truncdfdi2                 (rtx, rtx);
extern rtx        gen_vsx_fix_truncv4sfv4si2             (rtx, rtx);
extern rtx        gen_vsx_fix_truncv2dfv2di2             (rtx, rtx);
extern rtx        gen_vsx_fixuns_truncdfdi2              (rtx, rtx);
extern rtx        gen_vsx_fixuns_truncv4sfv4si2          (rtx, rtx);
extern rtx        gen_vsx_fixuns_truncv2dfv2di2          (rtx, rtx);
extern rtx        gen_vsx_xsrdpi                         (rtx, rtx);
extern rtx        gen_vsx_xvrspi                         (rtx, rtx);
extern rtx        gen_vsx_xvrdpi                         (rtx, rtx);
extern rtx        gen_vsx_xsrdpic                        (rtx, rtx);
extern rtx        gen_vsx_xvrspic                        (rtx, rtx);
extern rtx        gen_vsx_xvrdpic                        (rtx, rtx);
extern rtx        gen_vsx_btruncdf2                      (rtx, rtx);
extern rtx        gen_vsx_btruncv4sf2                    (rtx, rtx);
extern rtx        gen_vsx_btruncv2df2                    (rtx, rtx);
extern rtx        gen_vsx_floordf2                       (rtx, rtx);
extern rtx        gen_vsx_floorv4sf2                     (rtx, rtx);
extern rtx        gen_vsx_floorv2df2                     (rtx, rtx);
extern rtx        gen_vsx_ceildf2                        (rtx, rtx);
extern rtx        gen_vsx_ceilv4sf2                      (rtx, rtx);
extern rtx        gen_vsx_ceilv2df2                      (rtx, rtx);
extern rtx        gen_vsx_xscvdpsp                       (rtx, rtx);
extern rtx        gen_vsx_xvcvspdp                       (rtx, rtx);
extern rtx        gen_vsx_xvcvdpsp                       (rtx, rtx);
extern rtx        gen_vsx_xscvspdp                       (rtx, rtx);
extern rtx        gen_vsx_xscvdpsp_scalar                (rtx, rtx);
extern rtx        gen_vsx_xscvspdp_scalar2               (rtx, rtx);
extern rtx        gen_vsx_xvcvdpsxws                     (rtx, rtx);
extern rtx        gen_vsx_xvcvdpuxws                     (rtx, rtx);
extern rtx        gen_vsx_xvcvsxdsp                      (rtx, rtx);
extern rtx        gen_vsx_xvcvuxdsp                      (rtx, rtx);
extern rtx        gen_vsx_xvcvsxwdp                      (rtx, rtx);
extern rtx        gen_vsx_xvcvuxwdp                      (rtx, rtx);
extern rtx        gen_vsx_xvcvspsxds                     (rtx, rtx);
extern rtx        gen_vsx_xvcvspuxds                     (rtx, rtx);
extern rtx        gen_vsx_concat_v2df                    (rtx, rtx, rtx);
extern rtx        gen_vsx_concat_v2di                    (rtx, rtx, rtx);
extern rtx        gen_vsx_concat_v2sf                    (rtx, rtx, rtx);
extern rtx        gen_vsx_set_v2df                       (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_set_v2di                       (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_extract_v2df                   (rtx, rtx, rtx);
extern rtx        gen_vsx_extract_v2di                   (rtx, rtx, rtx);
extern rtx        gen_vsx_extract_v4sf                   (rtx, rtx, rtx);
extern rtx        gen_vsx_xxpermdi2_v2df_1               (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxpermdi2_v2di_1               (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_splat_v2df                     (rtx, rtx);
extern rtx        gen_vsx_splat_v2di                     (rtx, rtx);
extern rtx        gen_vsx_xxspltw_v4sf                   (rtx, rtx, rtx);
extern rtx        gen_vsx_xxspltw_v4si                   (rtx, rtx, rtx);
extern rtx        gen_vsx_xxmrghw_v4sf                   (rtx, rtx, rtx);
extern rtx        gen_vsx_xxmrghw_v4si                   (rtx, rtx, rtx);
extern rtx        gen_vsx_xxmrglw_v4sf                   (rtx, rtx, rtx);
extern rtx        gen_vsx_xxmrglw_v4si                   (rtx, rtx, rtx);
extern rtx        gen_vsx_xxsldwi_v16qi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxsldwi_v8hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxsldwi_v4si                   (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxsldwi_v2di                   (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxsldwi_v4sf                   (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxsldwi_v2df                   (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxsldwi_ti                     (rtx, rtx, rtx, rtx);
extern rtx        gen_get_vrsave_internal                (rtx);
extern rtx        gen_altivec_vabsduw                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vabsduh                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vabsdub                    (rtx, rtx, rtx);
extern rtx        gen_addv4si3                           (rtx, rtx, rtx);
extern rtx        gen_addv8hi3                           (rtx, rtx, rtx);
extern rtx        gen_addv16qi3                          (rtx, rtx, rtx);
extern rtx        gen_altivec_vaddcuw                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vadduws                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vadduhs                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vaddubs                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vaddsws                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vaddshs                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vaddsbs                    (rtx, rtx, rtx);
extern rtx        gen_subv4si3                           (rtx, rtx, rtx);
extern rtx        gen_subv8hi3                           (rtx, rtx, rtx);
extern rtx        gen_subv16qi3                          (rtx, rtx, rtx);
extern rtx        gen_altivec_vsubcuw                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vsubuws                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vsubuhs                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vsububs                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vsubsws                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vsubshs                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vsubsbs                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vavguw                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vavguh                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vavgub                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vavgsw                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vavgsh                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vavgsb                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vcmpbfp                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vmsumuhm                   (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmsumubm                   (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmsummhm                   (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmsummbm                   (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmsumshm                   (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmsumuhs                   (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmsumshs                   (rtx, rtx, rtx, rtx);
extern rtx        gen_umaxv4si3                          (rtx, rtx, rtx);
extern rtx        gen_umaxv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_umaxv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_smaxv4si3                          (rtx, rtx, rtx);
extern rtx        gen_smaxv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_smaxv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_uminv4si3                          (rtx, rtx, rtx);
extern rtx        gen_uminv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_uminv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_sminv4si3                          (rtx, rtx, rtx);
extern rtx        gen_sminv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_sminv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_altivec_vmhaddshs                  (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmhraddshs                 (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmladduhm                  (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vmrghb                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vmrghh                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vmrghw                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vmrglb                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vmrglh                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vmrglw                     (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_even_v16qi         (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_even_v16qi         (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_even_v8hi          (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_even_v8hi          (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_odd_v16qi          (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_odd_v16qi          (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_odd_v8hi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_odd_v8hi           (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkuhum                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkuwum                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkpx                      (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkshss                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkswss                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkuhus                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkshus                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkuwus                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vpkswus                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vsl                        (rtx, rtx, rtx);
extern rtx        gen_altivec_vslo                       (rtx, rtx, rtx);
extern rtx        gen_altivec_vsr                        (rtx, rtx, rtx);
extern rtx        gen_altivec_vsro                       (rtx, rtx, rtx);
extern rtx        gen_altivec_vsum4ubs                   (rtx, rtx, rtx);
extern rtx        gen_altivec_vsum4shs                   (rtx, rtx, rtx);
extern rtx        gen_altivec_vsum4sbs                   (rtx, rtx, rtx);
extern rtx        gen_altivec_vsum2sws                   (rtx, rtx, rtx);
extern rtx        gen_altivec_vsumsws                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vspltb                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vsplth                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vspltw                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vspltsf                    (rtx, rtx, rtx);
extern rtx        gen_altivec_vspltisw                   (rtx, rtx);
extern rtx        gen_altivec_vspltish                   (rtx, rtx);
extern rtx        gen_altivec_vspltisb                   (rtx, rtx);
extern rtx        gen_altivec_vperm_v4si                 (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v8hi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v16qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v4sf                 (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v2df                 (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v2di                 (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_ti                   (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v4si_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v8hi_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v16qi_uns            (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v4sf_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v2df_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_v2di_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vperm_ti_uns               (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vrfip                      (rtx, rtx);
extern rtx        gen_altivec_vrfin                      (rtx, rtx);
extern rtx        gen_altivec_vcfux                      (rtx, rtx, rtx);
extern rtx        gen_altivec_vcfsx                      (rtx, rtx, rtx);
extern rtx        gen_altivec_vctuxs                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vctsxs                     (rtx, rtx, rtx);
extern rtx        gen_altivec_vlogefp                    (rtx, rtx);
extern rtx        gen_altivec_vexptefp                   (rtx, rtx);
extern rtx        gen_altivec_vrefp                      (rtx, rtx);
extern rtx        gen_altivec_vsldoi_v4si                (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vsldoi_v8hi                (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vsldoi_v16qi               (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vsldoi_v4sf                (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vsldoi_v2df                (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vsldoi_v2di                (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vsldoi_ti                  (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_vupkhsb                    (rtx, rtx);
extern rtx        gen_altivec_vupkhpx                    (rtx, rtx);
extern rtx        gen_altivec_vupkhsh                    (rtx, rtx);
extern rtx        gen_altivec_vupklsb                    (rtx, rtx);
extern rtx        gen_altivec_vupklpx                    (rtx, rtx);
extern rtx        gen_altivec_vupklsh                    (rtx, rtx);
extern rtx        gen_altivec_vcmpbfp_p                  (rtx, rtx, rtx);
extern rtx        gen_altivec_mtvscr                     (rtx);
extern rtx        gen_altivec_mfvscr                     (rtx);
extern rtx        gen_altivec_dssall                     (void);
extern rtx        gen_altivec_dss                        (rtx);
extern rtx        gen_altivec_dst                        (rtx, rtx, rtx);
extern rtx        gen_altivec_dstt                       (rtx, rtx, rtx);
extern rtx        gen_altivec_dstst                      (rtx, rtx, rtx);
extern rtx        gen_altivec_dststt                     (rtx, rtx, rtx);
extern rtx        gen_altivec_lvsl                       (rtx, rtx);
extern rtx        gen_altivec_lvsr                       (rtx, rtx);
extern rtx        gen_altivec_lvewx                      (rtx, rtx);
extern rtx        gen_altivec_lvehx                      (rtx, rtx);
extern rtx        gen_altivec_lvebx                      (rtx, rtx);
extern rtx        gen_altivec_lvexwx                     (rtx, rtx);
extern rtx        gen_altivec_lvexhx                     (rtx, rtx);
extern rtx        gen_altivec_lvexbx                     (rtx, rtx);
extern rtx        gen_altivec_lvxl                       (rtx, rtx);
extern rtx        gen_altivec_lvx_v4si                   (rtx, rtx);
extern rtx        gen_altivec_lvx_v8hi                   (rtx, rtx);
extern rtx        gen_altivec_lvx_v16qi                  (rtx, rtx);
extern rtx        gen_altivec_lvx_v4sf                   (rtx, rtx);
extern rtx        gen_altivec_lvx_v2df                   (rtx, rtx);
extern rtx        gen_altivec_lvx_v2di                   (rtx, rtx);
extern rtx        gen_altivec_stvx_v4si                  (rtx, rtx);
extern rtx        gen_altivec_stvx_v8hi                  (rtx, rtx);
extern rtx        gen_altivec_stvx_v16qi                 (rtx, rtx);
extern rtx        gen_altivec_stvx_v4sf                  (rtx, rtx);
extern rtx        gen_altivec_stvx_v2df                  (rtx, rtx);
extern rtx        gen_altivec_stvx_v2di                  (rtx, rtx);
extern rtx        gen_altivec_stvxl                      (rtx, rtx);
extern rtx        gen_altivec_stvewx                     (rtx, rtx);
extern rtx        gen_altivec_stvehx                     (rtx, rtx);
extern rtx        gen_altivec_stvebx                     (rtx, rtx);
extern rtx        gen_altivec_stvexwx                    (rtx, rtx);
extern rtx        gen_altivec_stvexhx                    (rtx, rtx);
extern rtx        gen_altivec_stvexbx                    (rtx, rtx);
extern rtx        gen_vperm_v8hiv4si                     (rtx, rtx, rtx, rtx);
extern rtx        gen_vperm_v16qiv8hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_lvlx                       (rtx, rtx);
extern rtx        gen_altivec_lvlxl                      (rtx, rtx);
extern rtx        gen_altivec_lvrx                       (rtx, rtx);
extern rtx        gen_altivec_lvrxl                      (rtx, rtx);
extern rtx        gen_altivec_stvlx                      (rtx, rtx);
extern rtx        gen_altivec_stvlxl                     (rtx, rtx);
extern rtx        gen_altivec_stvrx                      (rtx, rtx);
extern rtx        gen_altivec_stvrxl                     (rtx, rtx);
extern rtx        gen_altivec_lvtlx                      (rtx, rtx);
extern rtx        gen_altivec_lvtlxl                     (rtx, rtx);
extern rtx        gen_altivec_lvtrx                      (rtx, rtx);
extern rtx        gen_altivec_lvtrxl                     (rtx, rtx);
extern rtx        gen_altivec_stvflx                     (rtx, rtx);
extern rtx        gen_altivec_stvflxl                    (rtx, rtx);
extern rtx        gen_altivec_stvfrx                     (rtx, rtx);
extern rtx        gen_altivec_stvfrxl                    (rtx, rtx);
extern rtx        gen_altivec_lvswx                      (rtx, rtx);
extern rtx        gen_altivec_lvswxl                     (rtx, rtx);
extern rtx        gen_altivec_lvsm                       (rtx, rtx);
extern rtx        gen_altivec_stvswx                     (rtx, rtx);
extern rtx        gen_altivec_stvswxl                    (rtx, rtx);
extern rtx        gen_spe_fixuns_truncdfsi2              (rtx, rtx);
extern rtx        gen_spe_extendsfdf2                    (rtx, rtx);
extern rtx        gen_spe_fixuns_truncsfsi2              (rtx, rtx);
extern rtx        gen_spe_fix_truncsfsi2                 (rtx, rtx);
extern rtx        gen_spe_fix_truncdfsi2                 (rtx, rtx);
extern rtx        gen_spe_floatunssisf2                  (rtx, rtx);
extern rtx        gen_spe_floatunssidf2                  (rtx, rtx);
extern rtx        gen_spe_floatsisf2                     (rtx, rtx);
extern rtx        gen_spe_floatsidf2                     (rtx, rtx);
extern rtx        gen_absv2si2                           (rtx, rtx);
extern rtx        gen_spe_evandc                         (rtx, rtx, rtx);
extern rtx        gen_andv2si3                           (rtx, rtx, rtx);
extern rtx        gen_spe_evcmpeq                        (rtx, rtx, rtx);
extern rtx        gen_spe_evcmpgts                       (rtx, rtx, rtx);
extern rtx        gen_spe_evcmpgtu                       (rtx, rtx, rtx);
extern rtx        gen_spe_evcmplts                       (rtx, rtx, rtx);
extern rtx        gen_spe_evcmpltu                       (rtx, rtx, rtx);
extern rtx        gen_spe_evfscmpeq                      (rtx, rtx, rtx);
extern rtx        gen_spe_evfscmpgt                      (rtx, rtx, rtx);
extern rtx        gen_spe_evfscmplt                      (rtx, rtx, rtx);
extern rtx        gen_spe_evfststeq                      (rtx, rtx, rtx);
extern rtx        gen_spe_evfststgt                      (rtx, rtx, rtx);
extern rtx        gen_spe_evfststlt                      (rtx, rtx, rtx);
extern rtx        gen_spe_evcntlsw                       (rtx, rtx);
extern rtx        gen_spe_evcntlzw                       (rtx, rtx);
extern rtx        gen_spe_eveqv                          (rtx, rtx, rtx);
extern rtx        gen_spe_evextsb                        (rtx, rtx);
extern rtx        gen_spe_evextsh                        (rtx, rtx);
extern rtx        gen_spe_evlhhesplat                    (rtx, rtx, rtx);
extern rtx        gen_spe_evlhhesplatx                   (rtx, rtx, rtx);
extern rtx        gen_spe_evlhhossplat                   (rtx, rtx, rtx);
extern rtx        gen_spe_evlhhossplatx                  (rtx, rtx, rtx);
extern rtx        gen_spe_evlhhousplat                   (rtx, rtx, rtx);
extern rtx        gen_spe_evlhhousplatx                  (rtx, rtx, rtx);
extern rtx        gen_spe_evlwhsplat                     (rtx, rtx, rtx);
extern rtx        gen_spe_evlwhsplatx                    (rtx, rtx, rtx);
extern rtx        gen_spe_evlwwsplat                     (rtx, rtx, rtx);
extern rtx        gen_spe_evlwwsplatx                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmergehi                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmergehilo                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmergelo                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmergelohi                    (rtx, rtx, rtx);
extern rtx        gen_spe_evnand                         (rtx, rtx, rtx);
extern rtx        gen_negv2si2                           (rtx, rtx);
extern rtx        gen_spe_evnor                          (rtx, rtx, rtx);
extern rtx        gen_spe_evorc                          (rtx, rtx, rtx);
extern rtx        gen_spe_evor                           (rtx, rtx, rtx);
extern rtx        gen_spe_evrlwi                         (rtx, rtx, rtx);
extern rtx        gen_spe_evrlw                          (rtx, rtx, rtx);
extern rtx        gen_spe_evrndw                         (rtx, rtx);
extern rtx        gen_spe_evsel                          (rtx, rtx, rtx, rtx);
extern rtx        gen_spe_evsel_fs                       (rtx, rtx, rtx, rtx);
extern rtx        gen_spe_evslwi                         (rtx, rtx, rtx);
extern rtx        gen_spe_evslw                          (rtx, rtx, rtx);
extern rtx        gen_spe_evsrwis                        (rtx, rtx, rtx);
extern rtx        gen_spe_evsrwiu                        (rtx, rtx, rtx);
extern rtx        gen_spe_evsrws                         (rtx, rtx, rtx);
extern rtx        gen_spe_evsrwu                         (rtx, rtx, rtx);
extern rtx        gen_xorv2si3                           (rtx, rtx, rtx);
extern rtx        gen_xorv4hi3                           (rtx, rtx, rtx);
extern rtx        gen_xorv1di3                           (rtx, rtx, rtx);
extern rtx        gen_spe_evfsabs                        (rtx, rtx);
extern rtx        gen_spe_evfsadd                        (rtx, rtx, rtx);
extern rtx        gen_spe_evfscfsf                       (rtx, rtx);
extern rtx        gen_spe_evfscfsi                       (rtx, rtx);
extern rtx        gen_spe_evfscfuf                       (rtx, rtx);
extern rtx        gen_spe_evfscfui                       (rtx, rtx);
extern rtx        gen_spe_evfsctsf                       (rtx, rtx);
extern rtx        gen_spe_evfsctsi                       (rtx, rtx);
extern rtx        gen_spe_evfsctsiz                      (rtx, rtx);
extern rtx        gen_spe_evfsctuf                       (rtx, rtx);
extern rtx        gen_spe_evfsctui                       (rtx, rtx);
extern rtx        gen_spe_evfsctuiz                      (rtx, rtx);
extern rtx        gen_spe_evfsdiv                        (rtx, rtx, rtx);
extern rtx        gen_spe_evfsmul                        (rtx, rtx, rtx);
extern rtx        gen_spe_evfsnabs                       (rtx, rtx);
extern rtx        gen_spe_evfsneg                        (rtx, rtx);
extern rtx        gen_spe_evfssub                        (rtx, rtx, rtx);
extern rtx        gen_spe_evldd                          (rtx, rtx, rtx);
extern rtx        gen_spe_evlddx                         (rtx, rtx, rtx);
extern rtx        gen_spe_evldh                          (rtx, rtx, rtx);
extern rtx        gen_spe_evldhx                         (rtx, rtx, rtx);
extern rtx        gen_spe_evldw                          (rtx, rtx, rtx);
extern rtx        gen_spe_evldwx                         (rtx, rtx, rtx);
extern rtx        gen_spe_evlwhe                         (rtx, rtx, rtx);
extern rtx        gen_spe_evlwhex                        (rtx, rtx, rtx);
extern rtx        gen_spe_evlwhos                        (rtx, rtx, rtx);
extern rtx        gen_spe_evlwhosx                       (rtx, rtx, rtx);
extern rtx        gen_spe_evlwhou                        (rtx, rtx, rtx);
extern rtx        gen_spe_evlwhoux                       (rtx, rtx, rtx);
extern rtx        gen_spe_brinc                          (rtx, rtx, rtx);
extern rtx        gen_spe_evmhegsmfaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhegsmfan                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhegsmiaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhegsmian                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhegumiaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhegumian                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhesmfaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhesmfanw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhesmfa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmhesmf                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmhesmiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhesmianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhesmia                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmhesmi                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmhessfaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhessfanw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhessfa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmhessf                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmhessiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhessianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmheumiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmheumianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmheumia                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmheumi                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmheusiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmheusianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhogsmfaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhogsmfan                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhogsmiaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhogsmian                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhogumiaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhogumian                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhosmfaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhosmfanw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhosmfa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmhosmf                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmhosmiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhosmianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhosmia                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmhosmi                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmhossfaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhossfanw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhossfa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmhossf                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmhossiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhossianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhoumiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhoumianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhoumia                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmhoumi                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmhousiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmhousianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmmlssfa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmmlssf                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhsmfa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhsmf                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhsmia                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhsmi                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhssfa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhusian                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhssf                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhumia                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhumi                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlsmiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlsmianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlssiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlssianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlumiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlumianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlumia                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlumi                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlusiaaw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwlusianw                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwsmfaa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwsmfan                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwsmfa                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwsmf                        (rtx, rtx, rtx);
extern rtx        gen_spe_evmwsmiaa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwsmian                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwsmia                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwsmi                        (rtx, rtx, rtx);
extern rtx        gen_spe_evmwssfaa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwssfan                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwssfa                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwssf                        (rtx, rtx, rtx);
extern rtx        gen_spe_evmwumiaa                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwumian                      (rtx, rtx, rtx);
extern rtx        gen_spe_evmwumia                       (rtx, rtx, rtx);
extern rtx        gen_spe_evmwumi                        (rtx, rtx, rtx);
extern rtx        gen_addv2si3                           (rtx, rtx, rtx);
extern rtx        gen_spe_evaddusiaaw                    (rtx, rtx);
extern rtx        gen_spe_evaddumiaaw                    (rtx, rtx);
extern rtx        gen_spe_evaddssiaaw                    (rtx, rtx);
extern rtx        gen_spe_evaddsmiaaw                    (rtx, rtx);
extern rtx        gen_spe_evaddiw                        (rtx, rtx, rtx);
extern rtx        gen_spe_evsubifw                       (rtx, rtx, rtx);
extern rtx        gen_subv2si3                           (rtx, rtx, rtx);
extern rtx        gen_spe_evsubfusiaaw                   (rtx, rtx);
extern rtx        gen_spe_evsubfumiaaw                   (rtx, rtx);
extern rtx        gen_spe_evsubfssiaaw                   (rtx, rtx);
extern rtx        gen_spe_evsubfsmiaaw                   (rtx, rtx);
extern rtx        gen_spe_evmra                          (rtx, rtx);
extern rtx        gen_divv2si3                           (rtx, rtx, rtx);
extern rtx        gen_spe_evdivwu                        (rtx, rtx, rtx);
extern rtx        gen_spe_evsplatfi                      (rtx, rtx);
extern rtx        gen_spe_evsplati                       (rtx, rtx);
extern rtx        gen_spe_evstdd                         (rtx, rtx, rtx);
extern rtx        gen_spe_evstddx                        (rtx, rtx, rtx);
extern rtx        gen_spe_evstdh                         (rtx, rtx, rtx);
extern rtx        gen_spe_evstdhx                        (rtx, rtx, rtx);
extern rtx        gen_spe_evstdw                         (rtx, rtx, rtx);
extern rtx        gen_spe_evstdwx                        (rtx, rtx, rtx);
extern rtx        gen_spe_evstwhe                        (rtx, rtx, rtx);
extern rtx        gen_spe_evstwhex                       (rtx, rtx, rtx);
extern rtx        gen_spe_evstwho                        (rtx, rtx, rtx);
extern rtx        gen_spe_evstwhox                       (rtx, rtx, rtx);
extern rtx        gen_spe_evstwwe                        (rtx, rtx, rtx);
extern rtx        gen_spe_evstwwex                       (rtx, rtx, rtx);
extern rtx        gen_spe_evstwwo                        (rtx, rtx, rtx);
extern rtx        gen_spe_evstwwox                       (rtx, rtx, rtx);
extern rtx        gen_mov_sidf_e500_subreg0              (rtx, rtx);
extern rtx        gen_mov_siv4hi_e500_subreg0            (rtx, rtx);
extern rtx        gen_mov_siv2sf_e500_subreg0            (rtx, rtx);
extern rtx        gen_mov_siv1di_e500_subreg0            (rtx, rtx);
extern rtx        gen_mov_siv2si_e500_subreg0            (rtx, rtx);
extern rtx        gen_mov_sitf_e500_subreg0              (rtx, rtx);
extern rtx        gen_spe_truncdfsf2                     (rtx, rtx);
extern rtx        gen_spe_absdf2                         (rtx, rtx);
extern rtx        gen_spe_nabsdf2                        (rtx, rtx);
extern rtx        gen_spe_negdf2                         (rtx, rtx);
extern rtx        gen_spe_adddf3                         (rtx, rtx, rtx);
extern rtx        gen_spe_subdf3                         (rtx, rtx, rtx);
extern rtx        gen_spe_muldf3                         (rtx, rtx, rtx);
extern rtx        gen_spe_divdf3                         (rtx, rtx, rtx);
extern rtx        gen_spe_trunctfdf2_internal1           (rtx, rtx);
extern rtx        gen_spe_trunctfsf2                     (rtx, rtx);
extern rtx        gen_spe_extenddftf2                    (rtx, rtx);
extern rtx        gen_spe_fix_trunctfsi2_internal        (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_spe_negtf2_internal                (rtx, rtx);
extern rtx        gen_spe_evmwhssfaa                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhssmaa                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhsmfaa                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhsmiaa                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhusiaa                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhumiaa                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhssfan                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhssian                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhsmfan                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhsmian                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhumian                     (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhgssfaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhgsmfaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhgsmiaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhgumiaa                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhgssfan                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhgsmfan                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhgsmian                    (rtx, rtx, rtx);
extern rtx        gen_spe_evmwhgumian                    (rtx, rtx, rtx);
extern rtx        gen_spe_mtspefscr                      (rtx);
extern rtx        gen_spe_mfspefscr                      (rtx);
extern rtx        gen_e500_flip_gt_bit                   (rtx, rtx);
extern rtx        gen_cmpsfeq_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tstsfeq_gpr                        (rtx, rtx, rtx);
extern rtx        gen_cmpsfgt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tstsfgt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_cmpsflt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tstsflt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_cmpdfeq_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tstdfeq_gpr                        (rtx, rtx, rtx);
extern rtx        gen_cmpdfgt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tstdfgt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_cmpdflt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tstdflt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_cmptfeq_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tsttfeq_gpr                        (rtx, rtx, rtx);
extern rtx        gen_cmptfgt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tsttfgt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_cmptflt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_tsttflt_gpr                        (rtx, rtx, rtx);
extern rtx        gen_e500_cr_ior_compare                (rtx, rtx, rtx);
extern rtx        gen_movsd_hardfloat                    (rtx, rtx);
extern rtx        gen_movsd_softfloat                    (rtx, rtx);
extern rtx        gen_movsd_store                        (rtx, rtx);
extern rtx        gen_movsd_load                         (rtx, rtx);
extern rtx        gen_extendsddd2                        (rtx, rtx);
extern rtx        gen_truncddsd2                         (rtx, rtx);
extern rtx        gen_extendddtd2                        (rtx, rtx);
extern rtx        gen_trunctddd2                         (rtx, rtx);
extern rtx        gen_adddd3                             (rtx, rtx, rtx);
extern rtx        gen_addtd3                             (rtx, rtx, rtx);
extern rtx        gen_subdd3                             (rtx, rtx, rtx);
extern rtx        gen_subtd3                             (rtx, rtx, rtx);
extern rtx        gen_muldd3                             (rtx, rtx, rtx);
extern rtx        gen_multd3                             (rtx, rtx, rtx);
extern rtx        gen_divdd3                             (rtx, rtx, rtx);
extern rtx        gen_divtd3                             (rtx, rtx, rtx);
extern rtx        gen_floatdidd2                         (rtx, rtx);
extern rtx        gen_floatditd2                         (rtx, rtx);
extern rtx        gen_ftruncdd2                          (rtx, rtx);
extern rtx        gen_fixdddi2                           (rtx, rtx);
extern rtx        gen_ftrunctd2                          (rtx, rtx);
extern rtx        gen_fixtddi2                           (rtx, rtx);
static inline rtx gen_paired_negv2sf2                    (rtx, rtx);
static inline rtx
gen_paired_negv2sf2(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_sqrtv2sf2                          (rtx, rtx);
static inline rtx
gen_sqrtv2sf2(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_paired_absv2sf2                    (rtx, rtx);
static inline rtx
gen_paired_absv2sf2(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_nabsv2sf2                          (rtx, rtx);
static inline rtx
gen_nabsv2sf2(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_paired_addv2sf3                    (rtx, rtx, rtx);
static inline rtx
gen_paired_addv2sf3(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_subv2sf3                    (rtx, rtx, rtx);
static inline rtx
gen_paired_subv2sf3(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_mulv2sf3                    (rtx, rtx, rtx);
static inline rtx
gen_paired_mulv2sf3(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_resv2sf2                           (rtx, rtx);
static inline rtx
gen_resv2sf2(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_paired_divv2sf3                    (rtx, rtx, rtx);
static inline rtx
gen_paired_divv2sf3(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_madds0                      (rtx, rtx, rtx, rtx);
static inline rtx
gen_paired_madds0(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
static inline rtx gen_paired_madds1                      (rtx, rtx, rtx, rtx);
static inline rtx
gen_paired_madds1(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
static inline rtx gen_selv2sf4                           (rtx, rtx, rtx, rtx, rtx);
static inline rtx
gen_selv2sf4(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d), rtx ARG_UNUSED (e))
{
  return 0;
}
static inline rtx gen_paired_stx                         (rtx, rtx);
static inline rtx
gen_paired_stx(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_paired_lx                          (rtx, rtx);
static inline rtx
gen_paired_lx(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_paired_cmpu0                       (rtx, rtx, rtx);
static inline rtx
gen_paired_cmpu0(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_cmpu1                       (rtx, rtx, rtx);
static inline rtx
gen_paired_cmpu1(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_merge00                     (rtx, rtx, rtx);
static inline rtx
gen_paired_merge00(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_merge01                     (rtx, rtx, rtx);
static inline rtx
gen_paired_merge01(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_merge10                     (rtx, rtx, rtx);
static inline rtx
gen_paired_merge10(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_merge11                     (rtx, rtx, rtx);
static inline rtx
gen_paired_merge11(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_sum0                        (rtx, rtx, rtx, rtx);
static inline rtx
gen_paired_sum0(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
static inline rtx gen_paired_sum1                        (rtx, rtx, rtx, rtx);
static inline rtx
gen_paired_sum1(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
static inline rtx gen_paired_muls0                       (rtx, rtx, rtx);
static inline rtx
gen_paired_muls0(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_paired_muls1                       (rtx, rtx, rtx);
static inline rtx
gen_paired_muls1(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_macho_high                         (rtx, rtx);
static inline rtx
gen_macho_high(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_macho_low                          (rtx, rtx, rtx);
static inline rtx
gen_macho_low(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_load_macho_picbase                 (rtx);
extern rtx        gen_macho_correct_pic                  (rtx, rtx, rtx, rtx);
extern rtx        gen_zero_extendqidi2                   (rtx, rtx);
extern rtx        gen_zero_extendhidi2                   (rtx, rtx);
extern rtx        gen_zero_extendsidi2                   (rtx, rtx);
extern rtx        gen_extendhidi2                        (rtx, rtx);
extern rtx        gen_extendsidi2                        (rtx, rtx);
extern rtx        gen_zero_extendqisi2                   (rtx, rtx);
extern rtx        gen_zero_extendhisi2                   (rtx, rtx);
extern rtx        gen_extendhisi2                        (rtx, rtx);
extern rtx        gen_strlensi                           (rtx, rtx, rtx, rtx);
extern rtx        gen_addsi3                             (rtx, rtx, rtx);
extern rtx        gen_adddi3                             (rtx, rtx, rtx);
extern rtx        gen_subsi3                             (rtx, rtx, rtx);
extern rtx        gen_subdi3                             (rtx, rtx, rtx);
extern rtx        gen_negsi2                             (rtx, rtx);
extern rtx        gen_negdi2                             (rtx, rtx);
extern rtx        gen_ctzsi2                             (rtx, rtx);
extern rtx        gen_ctzdi2                             (rtx, rtx);
extern rtx        gen_ffssi2                             (rtx, rtx);
extern rtx        gen_ffsdi2                             (rtx, rtx);
extern rtx        gen_popcountsi2                        (rtx, rtx);
extern rtx        gen_popcountdi2                        (rtx, rtx);
extern rtx        gen_paritysi2                          (rtx, rtx);
extern rtx        gen_paritydi2                          (rtx, rtx);
extern rtx        gen_bswaphi2                           (rtx, rtx);
extern rtx        gen_bswapsi2                           (rtx, rtx);
extern rtx        gen_bswapdi2                           (rtx, rtx);
extern rtx        gen_divsi3                             (rtx, rtx, rtx);
extern rtx        gen_divdi3                             (rtx, rtx, rtx);
extern rtx        gen_modsi3                             (rtx, rtx, rtx);
extern rtx        gen_moddi3                             (rtx, rtx, rtx);
extern rtx        gen_andsi3                             (rtx, rtx, rtx);
extern rtx        gen_iorsi3                             (rtx, rtx, rtx);
extern rtx        gen_xorsi3                             (rtx, rtx, rtx);
extern rtx        gen_insv                               (rtx, rtx, rtx, rtx);
extern rtx        gen_extzv                              (rtx, rtx, rtx, rtx);
extern rtx        gen_recipsf3                           (rtx, rtx, rtx);
extern rtx        gen_recipdf3                           (rtx, rtx, rtx);
extern rtx        gen_recipv4sf3                         (rtx, rtx, rtx);
extern rtx        gen_recipv2df3                         (rtx, rtx, rtx);
extern rtx        gen_rsqrtsf2                           (rtx, rtx);
extern rtx        gen_rsqrtdf2                           (rtx, rtx);
extern rtx        gen_rsqrtv4sf2                         (rtx, rtx);
extern rtx        gen_rsqrtv2df2                         (rtx, rtx);
extern rtx        gen_extendsfdf2                        (rtx, rtx);
extern rtx        gen_truncdfsf2                         (rtx, rtx);
extern rtx        gen_negsf2                             (rtx, rtx);
extern rtx        gen_abssf2                             (rtx, rtx);
extern rtx        gen_addsf3                             (rtx, rtx, rtx);
extern rtx        gen_subsf3                             (rtx, rtx, rtx);
extern rtx        gen_mulsf3                             (rtx, rtx, rtx);
extern rtx        gen_divsf3                             (rtx, rtx, rtx);
extern rtx        gen_sqrtsf2                            (rtx, rtx);
extern rtx        gen_signbittf2                         (rtx, rtx);
extern rtx        gen_copysignsf3                        (rtx, rtx, rtx);
extern rtx        gen_copysigndf3                        (rtx, rtx, rtx);
extern rtx        gen_smaxsf3                            (rtx, rtx, rtx);
extern rtx        gen_sminsf3                            (rtx, rtx, rtx);
extern rtx        gen_movsicc                            (rtx, rtx, rtx, rtx);
extern rtx        gen_movdicc                            (rtx, rtx, rtx, rtx);
extern rtx        gen_movsfcc                            (rtx, rtx, rtx, rtx);
extern rtx        gen_negdf2                             (rtx, rtx);
extern rtx        gen_absdf2                             (rtx, rtx);
extern rtx        gen_adddf3                             (rtx, rtx, rtx);
extern rtx        gen_subdf3                             (rtx, rtx, rtx);
extern rtx        gen_muldf3                             (rtx, rtx, rtx);
extern rtx        gen_divdf3                             (rtx, rtx, rtx);
extern rtx        gen_sqrtdf2                            (rtx, rtx);
extern rtx        gen_smaxdf3                            (rtx, rtx, rtx);
extern rtx        gen_smindf3                            (rtx, rtx, rtx);
extern rtx        gen_movdfcc                            (rtx, rtx, rtx, rtx);
extern rtx        gen_floatsidf2                         (rtx, rtx);
extern rtx        gen_floatunssisf2                      (rtx, rtx);
extern rtx        gen_floatunssidf2                      (rtx, rtx);
extern rtx        gen_fix_truncsfsi2                     (rtx, rtx);
extern rtx        gen_fix_truncdfsi2                     (rtx, rtx);
extern rtx        gen_fix_truncsfdi2                     (rtx, rtx);
extern rtx        gen_fix_truncdfdi2                     (rtx, rtx);
extern rtx        gen_fixuns_truncsfsi2                  (rtx, rtx);
extern rtx        gen_fixuns_truncdfsi2                  (rtx, rtx);
extern rtx        gen_fixuns_truncsfdi2                  (rtx, rtx);
extern rtx        gen_fixuns_truncdfdi2                  (rtx, rtx);
extern rtx        gen_btruncsf2                          (rtx, rtx);
extern rtx        gen_btruncdf2                          (rtx, rtx);
extern rtx        gen_ceilsf2                            (rtx, rtx);
extern rtx        gen_ceildf2                            (rtx, rtx);
extern rtx        gen_floorsf2                           (rtx, rtx);
extern rtx        gen_floordf2                           (rtx, rtx);
extern rtx        gen_floatsisf2                         (rtx, rtx);
extern rtx        gen_floatdidf2                         (rtx, rtx);
extern rtx        gen_floatunsdidf2                      (rtx, rtx);
extern rtx        gen_floatdisf2                         (rtx, rtx);
extern rtx        gen_floatdisf2_internal2               (rtx, rtx, rtx);
extern rtx        gen_floatunsdisf2                      (rtx, rtx);
extern rtx        gen_mulditi3                           (rtx, rtx, rtx);
extern rtx        gen_umulditi3                          (rtx, rtx, rtx);
extern rtx        gen_ashldi3                            (rtx, rtx, rtx);
extern rtx        gen_lshrdi3                            (rtx, rtx, rtx);
extern rtx        gen_ashrdi3                            (rtx, rtx, rtx);
extern rtx        gen_anddi3                             (rtx, rtx, rtx);
extern rtx        gen_iordi3                             (rtx, rtx, rtx);
extern rtx        gen_xordi3                             (rtx, rtx, rtx);
extern rtx        gen_movsi_got                          (rtx, rtx);
extern rtx        gen_movqi                              (rtx, rtx);
extern rtx        gen_movhi                              (rtx, rtx);
extern rtx        gen_movsi                              (rtx, rtx);
extern rtx        gen_movdi                              (rtx, rtx);
extern rtx        gen_movti                              (rtx, rtx);
extern rtx        gen_movcc                              (rtx, rtx);
extern rtx        gen_movsf                              (rtx, rtx);
extern rtx        gen_movdf                              (rtx, rtx);
extern rtx        gen_reload_si_store                    (rtx, rtx, rtx);
extern rtx        gen_reload_di_store                    (rtx, rtx, rtx);
extern rtx        gen_reload_si_load                     (rtx, rtx, rtx);
extern rtx        gen_reload_di_load                     (rtx, rtx, rtx);
extern rtx        gen_movtf                              (rtx, rtx);
extern rtx        gen_extenddftf2                        (rtx, rtx);
extern rtx        gen_extenddftf2_fprs                   (rtx, rtx);
extern rtx        gen_extendsftf2                        (rtx, rtx);
extern rtx        gen_trunctfdf2                         (rtx, rtx);
extern rtx        gen_trunctfsf2                         (rtx, rtx);
extern rtx        gen_floatsitf2                         (rtx, rtx);
extern rtx        gen_fix_trunctfsi2                     (rtx, rtx);
extern rtx        gen_fix_trunctfsi2_fprs                (rtx, rtx);
extern rtx        gen_negtf2                             (rtx, rtx);
extern rtx        gen_abstf2                             (rtx, rtx);
extern rtx        gen_abstf2_internal                    (rtx, rtx, rtx);
extern rtx        gen_load_multiple                      (rtx, rtx, rtx);
extern rtx        gen_store_multiple                     (rtx, rtx, rtx);
extern rtx        gen_setmemsi                           (rtx, rtx, rtx, rtx);
extern rtx        gen_movmemsi                           (rtx, rtx, rtx, rtx);
extern rtx        gen_movmemsi_8reg                      (rtx, rtx, rtx, rtx);
extern rtx        gen_movmemsi_6reg                      (rtx, rtx, rtx, rtx);
extern rtx        gen_movmemsi_4reg                      (rtx, rtx, rtx, rtx);
extern rtx        gen_movmemsi_2reg                      (rtx, rtx, rtx, rtx);
extern rtx        gen_movmemsi_1reg                      (rtx, rtx, rtx, rtx);
static inline rtx gen_tls_get_tpointer                   (rtx);
static inline rtx
gen_tls_get_tpointer(rtx ARG_UNUSED (a))
{
  return 0;
}
static inline rtx gen_tls_get_addrsi                     (rtx, rtx, rtx);
static inline rtx
gen_tls_get_addrsi(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_tls_get_addrdi                     (rtx, rtx, rtx);
static inline rtx
gen_tls_get_addrdi(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_allocate_stack                     (rtx, rtx);
extern rtx        gen_save_stack_function                (rtx, rtx);
extern rtx        gen_restore_stack_function             (rtx, rtx);
extern rtx        gen_restore_stack_block                (rtx, rtx);
extern rtx        gen_save_stack_nonlocal                (rtx, rtx);
extern rtx        gen_restore_stack_nonlocal             (rtx, rtx);
extern rtx        gen_load_toc_v4_PIC_1                  (rtx);
extern rtx        gen_load_toc_v4_PIC_1b                 (rtx, rtx);
extern rtx        gen_builtin_setjmp_receiver            (rtx);
#define GEN_CALL(A, B, C, D) gen_call ((A), (B), (C))
extern rtx        gen_call                               (rtx, rtx, rtx);
#define GEN_CALL_VALUE(A, B, C, D, E) gen_call_value ((A), (B), (C), (D))
extern rtx        gen_call_value                         (rtx, rtx, rtx, rtx);
extern rtx        gen_untyped_call                       (rtx, rtx, rtx);
#define GEN_SIBCALL(A, B, C, D) gen_sibcall ((A), (B), (C))
extern rtx        gen_sibcall                            (rtx, rtx, rtx);
#define GEN_SIBCALL_VALUE(A, B, C, D, E) gen_sibcall_value ((A), (B), (C), (D))
extern rtx        gen_sibcall_value                      (rtx, rtx, rtx, rtx);
extern rtx        gen_sibcall_epilogue                   (void);
extern rtx        gen_probe_stack                        (rtx);
extern rtx        gen_cbranchsi4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdi4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchtf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdd4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchtd4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresi4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoredi4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoredf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoretf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoredd4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoretd4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_stack_protect_set                  (rtx, rtx);
extern rtx        gen_stack_protect_test                 (rtx, rtx, rtx);
extern rtx        gen_indirect_jump                      (rtx);
extern rtx        gen_tablejump                          (rtx, rtx);
extern rtx        gen_tablejumpsi                        (rtx, rtx);
extern rtx        gen_tablejumpdi                        (rtx, rtx);
extern rtx        gen_doloop_end                         (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_ctrsi                              (rtx, rtx);
extern rtx        gen_ctrdi                              (rtx, rtx);
extern rtx        gen_ctrapsi4                           (rtx, rtx, rtx, rtx);
extern rtx        gen_ctrapdi4                           (rtx, rtx, rtx, rtx);
extern rtx        gen_prologue                           (void);
extern rtx        gen_epilogue                           (void);
extern rtx        gen_movsi_to_cr_one                    (rtx, rtx);
extern rtx        gen_eh_return                          (rtx);
extern rtx        gen_fmasf4                             (rtx, rtx, rtx, rtx);
extern rtx        gen_fmadf4                             (rtx, rtx, rtx, rtx);
static inline rtx gen_fmav2sf4                           (rtx, rtx, rtx, rtx);
static inline rtx
gen_fmav2sf4(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
extern rtx        gen_fmav4sf4                           (rtx, rtx, rtx, rtx);
extern rtx        gen_fmav2df4                           (rtx, rtx, rtx, rtx);
extern rtx        gen_fmssf4                             (rtx, rtx, rtx, rtx);
extern rtx        gen_fmsdf4                             (rtx, rtx, rtx, rtx);
static inline rtx gen_fmsv2sf4                           (rtx, rtx, rtx, rtx);
static inline rtx
gen_fmsv2sf4(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
extern rtx        gen_fmsv4sf4                           (rtx, rtx, rtx, rtx);
extern rtx        gen_fmsv2df4                           (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmasf4                            (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmadf4                            (rtx, rtx, rtx, rtx);
static inline rtx gen_fnmav2sf4                          (rtx, rtx, rtx, rtx);
static inline rtx
gen_fnmav2sf4(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
extern rtx        gen_fnmav4sf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmav2df4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmssf4                            (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmsdf4                            (rtx, rtx, rtx, rtx);
static inline rtx gen_fnmsv2sf4                          (rtx, rtx, rtx, rtx);
static inline rtx
gen_fnmsv2sf4(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
extern rtx        gen_fnmsv4sf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmsv2df4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_nfmasf4                            (rtx, rtx, rtx, rtx);
extern rtx        gen_nfmadf4                            (rtx, rtx, rtx, rtx);
static inline rtx gen_nfmav2sf4                          (rtx, rtx, rtx, rtx);
static inline rtx
gen_nfmav2sf4(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
extern rtx        gen_nfmav4sf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_nfmav2df4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_nfmssf4                            (rtx, rtx, rtx, rtx);
extern rtx        gen_nfmsdf4                            (rtx, rtx, rtx, rtx);
static inline rtx gen_nfmsv2sf4                          (rtx, rtx, rtx, rtx);
static inline rtx
gen_nfmsv2sf4(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
extern rtx        gen_nfmsv4sf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_nfmsv2df4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_rs6000_get_timebase                (rtx);
extern rtx        gen_mem_thread_fence                   (rtx);
extern rtx        gen_hwsync                             (void);
extern rtx        gen_lwsync                             (void);
extern rtx        gen_atomic_loadqi                      (rtx, rtx, rtx);
extern rtx        gen_atomic_loadhi                      (rtx, rtx, rtx);
extern rtx        gen_atomic_loadsi                      (rtx, rtx, rtx);
extern rtx        gen_atomic_loaddi                      (rtx, rtx, rtx);
extern rtx        gen_atomic_storeqi                     (rtx, rtx, rtx);
extern rtx        gen_atomic_storehi                     (rtx, rtx, rtx);
extern rtx        gen_atomic_storesi                     (rtx, rtx, rtx);
extern rtx        gen_atomic_storedi                     (rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swapqi          (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swaphi          (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swapsi          (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swapdi          (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangeqi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangehi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangesi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangedi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_addqi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_subqi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_orqi                        (rtx, rtx, rtx);
extern rtx        gen_atomic_xorqi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_andqi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_addhi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_subhi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_orhi                        (rtx, rtx, rtx);
extern rtx        gen_atomic_xorhi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_andhi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_addsi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_subsi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_orsi                        (rtx, rtx, rtx);
extern rtx        gen_atomic_xorsi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_andsi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_adddi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_subdi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_ordi                        (rtx, rtx, rtx);
extern rtx        gen_atomic_xordi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_anddi                       (rtx, rtx, rtx);
extern rtx        gen_atomic_nandqi                      (rtx, rtx, rtx);
extern rtx        gen_atomic_nandhi                      (rtx, rtx, rtx);
extern rtx        gen_atomic_nandsi                      (rtx, rtx, rtx);
extern rtx        gen_atomic_nanddi                      (rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_addqi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_subqi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_orqi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_xorqi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_andqi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_addhi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_subhi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_orhi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_xorhi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_andhi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_addsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_subsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_orsi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_xorsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_andsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_adddi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_subdi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_ordi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_xordi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_anddi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_nandqi                (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_nandhi                (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_nandsi                (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_nanddi                (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_add_fetchqi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_sub_fetchqi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_or_fetchqi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_xor_fetchqi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_and_fetchqi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_add_fetchhi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_sub_fetchhi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_or_fetchhi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_xor_fetchhi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_and_fetchhi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_add_fetchsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_sub_fetchsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_or_fetchsi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_xor_fetchsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_and_fetchsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_add_fetchdi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_sub_fetchdi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_or_fetchdi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_xor_fetchdi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_and_fetchdi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_nand_fetchqi                (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_nand_fetchhi                (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_nand_fetchsi                (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_nand_fetchdi                (rtx, rtx, rtx, rtx);
extern rtx        gen_movv16qi                           (rtx, rtx);
extern rtx        gen_movv8hi                            (rtx, rtx);
extern rtx        gen_movv4si                            (rtx, rtx);
extern rtx        gen_movv2di                            (rtx, rtx);
extern rtx        gen_movv4sf                            (rtx, rtx);
extern rtx        gen_movv2df                            (rtx, rtx);
extern rtx        gen_vector_load_v16qi                  (rtx, rtx);
extern rtx        gen_vector_load_v8hi                   (rtx, rtx);
extern rtx        gen_vector_load_v4si                   (rtx, rtx);
extern rtx        gen_vector_load_v2di                   (rtx, rtx);
extern rtx        gen_vector_load_v4sf                   (rtx, rtx);
extern rtx        gen_vector_load_v2df                   (rtx, rtx);
extern rtx        gen_vector_store_v16qi                 (rtx, rtx);
extern rtx        gen_vector_store_v8hi                  (rtx, rtx);
extern rtx        gen_vector_store_v4si                  (rtx, rtx);
extern rtx        gen_vector_store_v2di                  (rtx, rtx);
extern rtx        gen_vector_store_v4sf                  (rtx, rtx);
extern rtx        gen_vector_store_v2df                  (rtx, rtx);
extern rtx        gen_vector_altivec_load_v16qi          (rtx, rtx);
extern rtx        gen_vector_altivec_load_v8hi           (rtx, rtx);
extern rtx        gen_vector_altivec_load_v4si           (rtx, rtx);
extern rtx        gen_vector_altivec_load_v2di           (rtx, rtx);
extern rtx        gen_vector_altivec_load_v4sf           (rtx, rtx);
extern rtx        gen_vector_altivec_load_v2df           (rtx, rtx);
extern rtx        gen_vector_altivec_store_v16qi         (rtx, rtx);
extern rtx        gen_vector_altivec_store_v8hi          (rtx, rtx);
extern rtx        gen_vector_altivec_store_v4si          (rtx, rtx);
extern rtx        gen_vector_altivec_store_v2di          (rtx, rtx);
extern rtx        gen_vector_altivec_store_v4sf          (rtx, rtx);
extern rtx        gen_vector_altivec_store_v2df          (rtx, rtx);
extern rtx        gen_reload_v16qi_si_store              (rtx, rtx, rtx);
extern rtx        gen_reload_v16qi_di_store              (rtx, rtx, rtx);
extern rtx        gen_reload_v8hi_si_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v8hi_di_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v4si_si_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v4si_di_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v2di_si_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v2di_di_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v4sf_si_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v4sf_di_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v2df_si_store               (rtx, rtx, rtx);
extern rtx        gen_reload_v2df_di_store               (rtx, rtx, rtx);
extern rtx        gen_reload_df_si_store                 (rtx, rtx, rtx);
extern rtx        gen_reload_df_di_store                 (rtx, rtx, rtx);
extern rtx        gen_reload_ti_si_store                 (rtx, rtx, rtx);
extern rtx        gen_reload_ti_di_store                 (rtx, rtx, rtx);
extern rtx        gen_reload_v16qi_si_load               (rtx, rtx, rtx);
extern rtx        gen_reload_v16qi_di_load               (rtx, rtx, rtx);
extern rtx        gen_reload_v8hi_si_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v8hi_di_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v4si_si_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v4si_di_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v2di_si_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v2di_di_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v4sf_si_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v4sf_di_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v2df_si_load                (rtx, rtx, rtx);
extern rtx        gen_reload_v2df_di_load                (rtx, rtx, rtx);
extern rtx        gen_reload_df_si_load                  (rtx, rtx, rtx);
extern rtx        gen_reload_df_di_load                  (rtx, rtx, rtx);
extern rtx        gen_reload_ti_si_load                  (rtx, rtx, rtx);
extern rtx        gen_reload_ti_di_load                  (rtx, rtx, rtx);
extern rtx        gen_addv4sf3                           (rtx, rtx, rtx);
extern rtx        gen_addv2df3                           (rtx, rtx, rtx);
extern rtx        gen_subv4sf3                           (rtx, rtx, rtx);
extern rtx        gen_subv2df3                           (rtx, rtx, rtx);
extern rtx        gen_mulv4sf3                           (rtx, rtx, rtx);
extern rtx        gen_mulv2df3                           (rtx, rtx, rtx);
extern rtx        gen_divv4sf3                           (rtx, rtx, rtx);
extern rtx        gen_divv2df3                           (rtx, rtx, rtx);
extern rtx        gen_negv4sf2                           (rtx, rtx);
extern rtx        gen_negv2df2                           (rtx, rtx);
extern rtx        gen_absv4sf2                           (rtx, rtx);
extern rtx        gen_absv2df2                           (rtx, rtx);
extern rtx        gen_sminv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_sminv2df3                          (rtx, rtx, rtx);
extern rtx        gen_smaxv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_smaxv2df3                          (rtx, rtx, rtx);
extern rtx        gen_sqrtv4sf2                          (rtx, rtx);
extern rtx        gen_sqrtv2df2                          (rtx, rtx);
extern rtx        gen_rsqrtev4sf2                        (rtx, rtx);
extern rtx        gen_rsqrtev2df2                        (rtx, rtx);
extern rtx        gen_rev4sf2                            (rtx, rtx);
extern rtx        gen_rev2df2                            (rtx, rtx);
extern rtx        gen_ftruncv4sf2                        (rtx, rtx);
extern rtx        gen_ftruncv2df2                        (rtx, rtx);
extern rtx        gen_vector_ceilv4sf2                   (rtx, rtx);
extern rtx        gen_vector_ceilv2df2                   (rtx, rtx);
extern rtx        gen_vector_floorv4sf2                  (rtx, rtx);
extern rtx        gen_vector_floorv2df2                  (rtx, rtx);
extern rtx        gen_vector_btruncv4sf2                 (rtx, rtx);
extern rtx        gen_vector_btruncv2df2                 (rtx, rtx);
extern rtx        gen_vector_copysignv4sf3               (rtx, rtx, rtx);
extern rtx        gen_vector_copysignv2df3               (rtx, rtx, rtx);
extern rtx        gen_vcondv4sfv4sf                      (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2dfv2df                      (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv16qiv16qi                    (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv8hiv8hi                      (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4siv4si                      (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4sfv4si                      (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4siv4sf                      (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv16qiv16qi                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv8hiv8hi                     (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4siv4si                     (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4sfv4si                     (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vector_eqv16qi                     (rtx, rtx, rtx);
extern rtx        gen_vector_eqv8hi                      (rtx, rtx, rtx);
extern rtx        gen_vector_eqv4si                      (rtx, rtx, rtx);
extern rtx        gen_vector_eqv4sf                      (rtx, rtx, rtx);
extern rtx        gen_vector_eqv2df                      (rtx, rtx, rtx);
extern rtx        gen_vector_gtv16qi                     (rtx, rtx, rtx);
extern rtx        gen_vector_gtv8hi                      (rtx, rtx, rtx);
extern rtx        gen_vector_gtv4si                      (rtx, rtx, rtx);
extern rtx        gen_vector_gtv4sf                      (rtx, rtx, rtx);
extern rtx        gen_vector_gtv2df                      (rtx, rtx, rtx);
extern rtx        gen_vector_gev16qi                     (rtx, rtx, rtx);
extern rtx        gen_vector_gev8hi                      (rtx, rtx, rtx);
extern rtx        gen_vector_gev4si                      (rtx, rtx, rtx);
extern rtx        gen_vector_gev4sf                      (rtx, rtx, rtx);
extern rtx        gen_vector_gev2df                      (rtx, rtx, rtx);
extern rtx        gen_vector_gtuv16qi                    (rtx, rtx, rtx);
extern rtx        gen_vector_gtuv8hi                     (rtx, rtx, rtx);
extern rtx        gen_vector_gtuv4si                     (rtx, rtx, rtx);
extern rtx        gen_vector_geuv16qi                    (rtx, rtx, rtx);
extern rtx        gen_vector_geuv8hi                     (rtx, rtx, rtx);
extern rtx        gen_vector_geuv4si                     (rtx, rtx, rtx);
extern rtx        gen_vector_select_v16qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v8hi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v4si                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v2di                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v4sf                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v2df                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_ti                   (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v16qi_uns            (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v8hi_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v4si_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v2di_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v4sf_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_v2df_uns             (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_select_ti_uns               (rtx, rtx, rtx, rtx);
extern rtx        gen_vector_eq_v16qi_p                  (rtx, rtx, rtx);
extern rtx        gen_vector_eq_v8hi_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_eq_v4si_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_eq_v4sf_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_eq_v2df_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_gt_v16qi_p                  (rtx, rtx, rtx);
extern rtx        gen_vector_gt_v8hi_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_gt_v4si_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_gt_v4sf_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_gt_v2df_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_ge_v4sf_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_ge_v2df_p                   (rtx, rtx, rtx);
extern rtx        gen_vector_gtu_v16qi_p                 (rtx, rtx, rtx);
extern rtx        gen_vector_gtu_v8hi_p                  (rtx, rtx, rtx);
extern rtx        gen_vector_gtu_v4si_p                  (rtx, rtx, rtx);
extern rtx        gen_cr6_test_for_zero                  (rtx);
extern rtx        gen_cr6_test_for_zero_reverse          (rtx);
extern rtx        gen_cr6_test_for_lt                    (rtx);
extern rtx        gen_cr6_test_for_lt_reverse            (rtx);
extern rtx        gen_xorv16qi3                          (rtx, rtx, rtx);
extern rtx        gen_xorv8hi3                           (rtx, rtx, rtx);
extern rtx        gen_xorv4si3                           (rtx, rtx, rtx);
extern rtx        gen_xorv2di3                           (rtx, rtx, rtx);
extern rtx        gen_xorv4sf3                           (rtx, rtx, rtx);
extern rtx        gen_xorv2df3                           (rtx, rtx, rtx);
extern rtx        gen_xorti3                             (rtx, rtx, rtx);
extern rtx        gen_iorv16qi3                          (rtx, rtx, rtx);
extern rtx        gen_iorv8hi3                           (rtx, rtx, rtx);
extern rtx        gen_iorv4si3                           (rtx, rtx, rtx);
extern rtx        gen_iorv2di3                           (rtx, rtx, rtx);
extern rtx        gen_iorv4sf3                           (rtx, rtx, rtx);
extern rtx        gen_iorv2df3                           (rtx, rtx, rtx);
extern rtx        gen_iorti3                             (rtx, rtx, rtx);
extern rtx        gen_andv16qi3                          (rtx, rtx, rtx);
extern rtx        gen_andv8hi3                           (rtx, rtx, rtx);
extern rtx        gen_andv4si3                           (rtx, rtx, rtx);
extern rtx        gen_andv2di3                           (rtx, rtx, rtx);
extern rtx        gen_andv4sf3                           (rtx, rtx, rtx);
extern rtx        gen_andv2df3                           (rtx, rtx, rtx);
extern rtx        gen_andti3                             (rtx, rtx, rtx);
extern rtx        gen_one_cmplv16qi2                     (rtx, rtx);
extern rtx        gen_one_cmplv8hi2                      (rtx, rtx);
extern rtx        gen_one_cmplv4si2                      (rtx, rtx);
extern rtx        gen_one_cmplv2di2                      (rtx, rtx);
extern rtx        gen_one_cmplv4sf2                      (rtx, rtx);
extern rtx        gen_one_cmplv2df2                      (rtx, rtx);
extern rtx        gen_one_cmplti2                        (rtx, rtx);
extern rtx        gen_norv16qi3                          (rtx, rtx, rtx);
extern rtx        gen_norv8hi3                           (rtx, rtx, rtx);
extern rtx        gen_norv4si3                           (rtx, rtx, rtx);
extern rtx        gen_norv2di3                           (rtx, rtx, rtx);
extern rtx        gen_norv4sf3                           (rtx, rtx, rtx);
extern rtx        gen_norv2df3                           (rtx, rtx, rtx);
extern rtx        gen_norti3                             (rtx, rtx, rtx);
extern rtx        gen_andcv16qi3                         (rtx, rtx, rtx);
extern rtx        gen_andcv8hi3                          (rtx, rtx, rtx);
extern rtx        gen_andcv4si3                          (rtx, rtx, rtx);
extern rtx        gen_andcv2di3                          (rtx, rtx, rtx);
extern rtx        gen_andcv4sf3                          (rtx, rtx, rtx);
extern rtx        gen_andcv2df3                          (rtx, rtx, rtx);
extern rtx        gen_andcti3                            (rtx, rtx, rtx);
extern rtx        gen_floatv4siv4sf2                     (rtx, rtx);
extern rtx        gen_floatv2div2df2                     (rtx, rtx);
extern rtx        gen_floatunsv4siv4sf2                  (rtx, rtx);
extern rtx        gen_floatunsv2div2df2                  (rtx, rtx);
extern rtx        gen_fix_truncv4sfv4si2                 (rtx, rtx);
extern rtx        gen_fix_truncv2dfv2di2                 (rtx, rtx);
extern rtx        gen_fixuns_truncv4sfv4si2              (rtx, rtx);
extern rtx        gen_fixuns_truncv2dfv2di2              (rtx, rtx);
extern rtx        gen_vec_initv16qi                      (rtx, rtx);
extern rtx        gen_vec_initv8hi                       (rtx, rtx);
extern rtx        gen_vec_initv4si                       (rtx, rtx);
extern rtx        gen_vec_initv2di                       (rtx, rtx);
extern rtx        gen_vec_initv4sf                       (rtx, rtx);
extern rtx        gen_vec_initv2df                       (rtx, rtx);
extern rtx        gen_vec_setv16qi                       (rtx, rtx, rtx);
extern rtx        gen_vec_setv8hi                        (rtx, rtx, rtx);
extern rtx        gen_vec_setv4si                        (rtx, rtx, rtx);
extern rtx        gen_vec_setv2di                        (rtx, rtx, rtx);
extern rtx        gen_vec_setv4sf                        (rtx, rtx, rtx);
extern rtx        gen_vec_setv2df                        (rtx, rtx, rtx);
extern rtx        gen_vec_extractv16qi                   (rtx, rtx, rtx);
extern rtx        gen_vec_extractv8hi                    (rtx, rtx, rtx);
extern rtx        gen_vec_extractv4si                    (rtx, rtx, rtx);
extern rtx        gen_vec_extractv2di                    (rtx, rtx, rtx);
extern rtx        gen_vec_extractv4sf                    (rtx, rtx, rtx);
extern rtx        gen_vec_extractv2df                    (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v2df                (rtx, rtx, rtx);
extern rtx        gen_vec_pack_sfix_trunc_v2df           (rtx, rtx, rtx);
extern rtx        gen_vec_pack_ufix_trunc_v2df           (rtx, rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v4sf                (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v4sf                (rtx, rtx);
extern rtx        gen_vec_unpacks_float_hi_v4si          (rtx, rtx);
extern rtx        gen_vec_unpacks_float_lo_v4si          (rtx, rtx);
extern rtx        gen_vec_unpacku_float_hi_v4si          (rtx, rtx);
extern rtx        gen_vec_unpacku_float_lo_v4si          (rtx, rtx);
extern rtx        gen_vec_realign_load_v16qi             (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_realign_load_v8hi              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_realign_load_v4si              (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_realign_load_v4sf              (rtx, rtx, rtx, rtx);
extern rtx        gen_movmisalignv4si                    (rtx, rtx);
extern rtx        gen_movmisalignv4sf                    (rtx, rtx);
extern rtx        gen_movmisalignv2di                    (rtx, rtx);
extern rtx        gen_movmisalignv2df                    (rtx, rtx);
extern rtx        gen_vec_shl_v16qi                      (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v8hi                       (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v4si                       (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v2di                       (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v4sf                       (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v2df                       (rtx, rtx, rtx);
extern rtx        gen_vec_shl_ti                         (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v16qi                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v8hi                       (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v4si                       (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v2di                       (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v4sf                       (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v2df                       (rtx, rtx, rtx);
extern rtx        gen_vec_shr_ti                         (rtx, rtx, rtx);
extern rtx        gen_vrotlv16qi3                        (rtx, rtx, rtx);
extern rtx        gen_vrotlv8hi3                         (rtx, rtx, rtx);
extern rtx        gen_vrotlv4si3                         (rtx, rtx, rtx);
extern rtx        gen_vashlv16qi3                        (rtx, rtx, rtx);
extern rtx        gen_vashlv8hi3                         (rtx, rtx, rtx);
extern rtx        gen_vashlv4si3                         (rtx, rtx, rtx);
extern rtx        gen_vlshrv16qi3                        (rtx, rtx, rtx);
extern rtx        gen_vlshrv8hi3                         (rtx, rtx, rtx);
extern rtx        gen_vlshrv4si3                         (rtx, rtx, rtx);
extern rtx        gen_vashrv16qi3                        (rtx, rtx, rtx);
extern rtx        gen_vashrv8hi3                         (rtx, rtx, rtx);
extern rtx        gen_vashrv4si3                         (rtx, rtx, rtx);
extern rtx        gen_reduc_splus_v2df                   (rtx, rtx);
extern rtx        gen_reduc_smin_v2df                    (rtx, rtx);
extern rtx        gen_reduc_smax_v2df                    (rtx, rtx);
extern rtx        gen_reduc_splus_v4sf                   (rtx, rtx);
extern rtx        gen_reduc_smin_v4sf                    (rtx, rtx);
extern rtx        gen_reduc_smax_v4sf                    (rtx, rtx);
extern rtx        gen_absv2sf2                           (rtx, rtx);
extern rtx        gen_negv2sf2                           (rtx, rtx);
extern rtx        gen_addv2sf3                           (rtx, rtx, rtx);
extern rtx        gen_subv2sf3                           (rtx, rtx, rtx);
extern rtx        gen_mulv2sf3                           (rtx, rtx, rtx);
extern rtx        gen_divv2sf3                           (rtx, rtx, rtx);
extern rtx        gen_vsx_load_v16qi                     (rtx, rtx);
extern rtx        gen_vsx_load_v8hi                      (rtx, rtx);
extern rtx        gen_vsx_load_v4si                      (rtx, rtx);
extern rtx        gen_vsx_load_v2di                      (rtx, rtx);
extern rtx        gen_vsx_load_v4sf                      (rtx, rtx);
extern rtx        gen_vsx_load_v2df                      (rtx, rtx);
extern rtx        gen_vsx_store_v16qi                    (rtx, rtx);
extern rtx        gen_vsx_store_v8hi                     (rtx, rtx);
extern rtx        gen_vsx_store_v4si                     (rtx, rtx);
extern rtx        gen_vsx_store_v2di                     (rtx, rtx);
extern rtx        gen_vsx_store_v4sf                     (rtx, rtx);
extern rtx        gen_vsx_store_v2df                     (rtx, rtx);
extern rtx        gen_vsx_tdivdf3_fg                     (rtx, rtx, rtx);
extern rtx        gen_vsx_tdivv4sf3_fg                   (rtx, rtx, rtx);
extern rtx        gen_vsx_tdivv2df3_fg                   (rtx, rtx, rtx);
extern rtx        gen_vsx_tdivdf3_fe                     (rtx, rtx, rtx);
extern rtx        gen_vsx_tdivv4sf3_fe                   (rtx, rtx, rtx);
extern rtx        gen_vsx_tdivv2df3_fe                   (rtx, rtx, rtx);
extern rtx        gen_vsx_tsqrtdf2_fg                    (rtx, rtx);
extern rtx        gen_vsx_tsqrtv4sf2_fg                  (rtx, rtx);
extern rtx        gen_vsx_tsqrtv2df2_fg                  (rtx, rtx);
extern rtx        gen_vsx_tsqrtdf2_fe                    (rtx, rtx);
extern rtx        gen_vsx_tsqrtv4sf2_fe                  (rtx, rtx);
extern rtx        gen_vsx_tsqrtv2df2_fe                  (rtx, rtx);
extern rtx        gen_vsx_xxpermdi_v16qi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxpermdi_v8hi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxpermdi_v4si                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxpermdi_v2di                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxpermdi_v4sf                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxpermdi_v2df                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_xxpermdi_ti                    (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_perm_constv2df                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_perm_constv2di                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vsx_mergel_v2df                    (rtx, rtx, rtx);
extern rtx        gen_vsx_mergel_v2di                    (rtx, rtx, rtx);
extern rtx        gen_vsx_mergeh_v2df                    (rtx, rtx, rtx);
extern rtx        gen_vsx_mergeh_v2di                    (rtx, rtx, rtx);
extern rtx        gen_altivec_mulv4sf3                   (rtx, rtx, rtx);
extern rtx        gen_mulv4si3                           (rtx, rtx, rtx);
extern rtx        gen_mulv8hi3                           (rtx, rtx, rtx);
extern rtx        gen_vec_permv16qi                      (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_perm_constv16qi                (rtx, rtx, rtx, rtx);
extern rtx        gen_altivec_copysign_v4sf3             (rtx, rtx, rtx);
extern rtx        gen_build_vector_mask_for_load         (rtx, rtx);
extern rtx        gen_absv4si2                           (rtx, rtx);
extern rtx        gen_absv8hi2                           (rtx, rtx);
extern rtx        gen_absv16qi2                          (rtx, rtx);
extern rtx        gen_altivec_absv4sf2                   (rtx, rtx);
extern rtx        gen_altivec_abss_v4si                  (rtx, rtx);
extern rtx        gen_altivec_abss_v8hi                  (rtx, rtx);
extern rtx        gen_altivec_abss_v16qi                 (rtx, rtx);
extern rtx        gen_reduc_splus_v8hi                   (rtx, rtx);
extern rtx        gen_reduc_splus_v16qi                  (rtx, rtx);
extern rtx        gen_reduc_uplus_v16qi                  (rtx, rtx);
extern rtx        gen_negv4si2                           (rtx, rtx);
extern rtx        gen_negv8hi2                           (rtx, rtx);
extern rtx        gen_negv16qi2                          (rtx, rtx);
extern rtx        gen_udot_prodv8hi                      (rtx, rtx, rtx, rtx);
extern rtx        gen_udot_prodv16qi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_sdot_prodv8hi                      (rtx, rtx, rtx, rtx);
extern rtx        gen_widen_usumv8hi3                    (rtx, rtx, rtx);
extern rtx        gen_widen_usumv16qi3                   (rtx, rtx, rtx);
extern rtx        gen_widen_ssumv16qi3                   (rtx, rtx, rtx);
extern rtx        gen_widen_ssumv8hi3                    (rtx, rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v16qi               (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v8hi                (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v16qi               (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v8hi                (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v16qi               (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v8hi                (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v16qi               (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v8hi                (rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v16qi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v16qi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v16qi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v16qi           (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v8hi            (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v8hi            (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v8hi            (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v8hi            (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v8hi                (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v4si                (rtx, rtx, rtx);
extern rtx        gen_altivec_negv4sf2                   (rtx, rtx);
extern rtx        gen_vec_unpacks_float_hi_v8hi          (rtx, rtx);
extern rtx        gen_vec_unpacks_float_lo_v8hi          (rtx, rtx);
extern rtx        gen_vec_unpacku_float_hi_v8hi          (rtx, rtx);
extern rtx        gen_vec_unpacku_float_lo_v8hi          (rtx, rtx);
extern rtx        gen_vec_perm_constv2si                 (rtx, rtx, rtx, rtx);
extern rtx        gen_spe_fix_trunctfsi2                 (rtx, rtx);
extern rtx        gen_spe_abstf2_cmp                     (rtx, rtx, rtx);
extern rtx        gen_spe_abstf2_tst                     (rtx, rtx, rtx);
extern rtx        gen_movv2si                            (rtx, rtx);
extern rtx        gen_movv1di                            (rtx, rtx);
extern rtx        gen_movv4hi                            (rtx, rtx);
extern rtx        gen_movv2sf                            (rtx, rtx);
extern rtx        gen_movsd                              (rtx, rtx);
extern rtx        gen_extendsdtd2                        (rtx, rtx);
extern rtx        gen_negdd2                             (rtx, rtx);
extern rtx        gen_absdd2                             (rtx, rtx);
extern rtx        gen_movdd                              (rtx, rtx);
extern rtx        gen_negtd2                             (rtx, rtx);
extern rtx        gen_abstd2                             (rtx, rtx);
extern rtx        gen_movtd                              (rtx, rtx);
static inline rtx gen_vec_perm_constv2sf                 (rtx, rtx, rtx, rtx);
static inline rtx
gen_vec_perm_constv2sf(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d))
{
  return 0;
}
static inline rtx gen_vec_initv2sf                       (rtx, rtx);
static inline rtx
gen_vec_initv2sf(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_sminv2sf3                          (rtx, rtx, rtx);
static inline rtx
gen_sminv2sf3(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_smaxv2sf3                          (rtx, rtx, rtx);
static inline rtx
gen_smaxv2sf3(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_reduc_smax_v2sf                    (rtx, rtx);
static inline rtx
gen_reduc_smax_v2sf(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_reduc_smin_v2sf                    (rtx, rtx);
static inline rtx
gen_reduc_smin_v2sf(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_reduc_splus_v2sf                   (rtx, rtx);
static inline rtx
gen_reduc_splus_v2sf(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_movmisalignv2sf                    (rtx, rtx);
static inline rtx
gen_movmisalignv2sf(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
static inline rtx gen_vcondv2sfv2sf                      (rtx, rtx, rtx, rtx, rtx, rtx);
static inline rtx
gen_vcondv2sfv2sf(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c), rtx ARG_UNUSED (d), rtx ARG_UNUSED (e), rtx ARG_UNUSED (f))
{
  return 0;
}

#endif /* GCC_INSN_FLAGS_H */
