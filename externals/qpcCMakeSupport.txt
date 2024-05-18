set(QPC_SRC_DIR ${CMS_QPC_TOP_DIR}/src)
set(QPC_INCLUDE_DIR ${CMS_QPC_TOP_DIR}/include)
set(QPC_QF_SRC_DIR ${QPC_SRC_DIR}/qf)

set(QPC_QF_SRCS
	${QPC_QF_SRC_DIR}/qep_hsm.c
	${QPC_QF_SRC_DIR}/qep_msm.c
	${QPC_QF_SRC_DIR}/qf_act.c
	${QPC_QF_SRC_DIR}/qf_actq.c
	${QPC_QF_SRC_DIR}/qf_defer.c
	${QPC_QF_SRC_DIR}/qf_dyn.c
	${QPC_QF_SRC_DIR}/qf_mem.c
	${QPC_QF_SRC_DIR}/qf_ps.c
	${QPC_QF_SRC_DIR}/qf_qact.c
	${QPC_QF_SRC_DIR}/qf_qeq.c
	${QPC_QF_SRC_DIR}/qf_qmact.c
	${QPC_QF_SRC_DIR}/qf_time.c)