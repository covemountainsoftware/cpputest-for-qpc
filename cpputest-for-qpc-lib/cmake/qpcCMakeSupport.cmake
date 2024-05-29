set(CMS_QPC_SRC_DIR ${CMS_QPC_TOP_DIR}/src)
set(CMS_QPC_INCLUDE_DIR ${CMS_QPC_TOP_DIR}/include)
set(CMS_QPC_QF_SRC_DIR ${CMS_QPC_SRC_DIR}/qf)

set(CMS_QPC_QF_SRCS
	${CMS_QPC_QF_SRC_DIR}/qep_hsm.c
	${CMS_QPC_QF_SRC_DIR}/qep_msm.c
	${CMS_QPC_QF_SRC_DIR}/qf_act.c
	${CMS_QPC_QF_SRC_DIR}/qf_actq.c
	${CMS_QPC_QF_SRC_DIR}/qf_defer.c
	${CMS_QPC_QF_SRC_DIR}/qf_dyn.c
	${CMS_QPC_QF_SRC_DIR}/qf_mem.c
	${CMS_QPC_QF_SRC_DIR}/qf_ps.c
	${CMS_QPC_QF_SRC_DIR}/qf_qact.c
	${CMS_QPC_QF_SRC_DIR}/qf_qeq.c
	${CMS_QPC_QF_SRC_DIR}/qf_qmact.c
	${CMS_QPC_QF_SRC_DIR}/qf_time.c)