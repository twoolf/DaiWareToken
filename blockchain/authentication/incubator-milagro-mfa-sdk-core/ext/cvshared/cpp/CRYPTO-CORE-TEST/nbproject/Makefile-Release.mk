#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1472/CvXcode.o \
	${OBJECTDIR}/_ext/1472/CvEntropyServer.o \
	${OBJECTDIR}/_ext/2003432284/octet_c.o \
	${OBJECTDIR}/_ext/1472/CvAesGcm.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/_ext/2003432284/big.o \
	${OBJECTDIR}/_ext/2003432284/eccsi_c.o \
	${OBJECTDIR}/_ext/2003432284/aesGcm.o \
	${OBJECTDIR}/_ext/1472/CvString.o \
	${OBJECTDIR}/_ext/1472/CvStrongRng.o \
	${OBJECTDIR}/_ext/2003432284/zzn2.o \
	${OBJECTDIR}/_ext/2003432284/sakke_bn_c.o \
	${OBJECTDIR}/_ext/1472/CvEccsi.o \
	${OBJECTDIR}/_ext/2003432284/ecp_c.o \
	${OBJECTDIR}/_ext/2003432284/zzn12a.o \
	${OBJECTDIR}/_ext/2003432284/ecdh_c.o \
	${OBJECTDIR}/_ext/1472/CvMikey.o \
	${OBJECTDIR}/_ext/2003432284/zzn.o \
	${OBJECTDIR}/_ext/2003432284/zzn4.o \
	${OBJECTDIR}/_ext/2003432284/ecn.o \
	${OBJECTDIR}/_ext/1472/CvSakke.o \
	${OBJECTDIR}/_ext/2003432284/pfc.o \
	${OBJECTDIR}/_ext/2003432284/csprng_c.o \
	${OBJECTDIR}/_ext/1472/CvEcdh.o \
	${OBJECTDIR}/_ext/2003432284/ecn2.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread ../../../MIRACL/miracl.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crypto-core-test

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crypto-core-test: ../../../MIRACL/miracl.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crypto-core-test: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crypto-core-test ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/1472/CvXcode.o: ../CvXcode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvXcode.o ../CvXcode.cpp

${OBJECTDIR}/_ext/1472/CvEntropyServer.o: ../CvEntropyServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvEntropyServer.o ../CvEntropyServer.cpp

${OBJECTDIR}/_ext/2003432284/octet_c.o: ../crypto-core/octet_c.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.c) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/octet_c.o ../crypto-core/octet_c.c

${OBJECTDIR}/_ext/1472/CvAesGcm.o: ../CvAesGcm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvAesGcm.o ../CvAesGcm.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/_ext/2003432284/big.o: ../crypto-core/big.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/big.o ../crypto-core/big.cpp

${OBJECTDIR}/_ext/2003432284/eccsi_c.o: ../crypto-core/eccsi_c.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.c) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/eccsi_c.o ../crypto-core/eccsi_c.c

${OBJECTDIR}/_ext/2003432284/aesGcm.o: ../crypto-core/aesGcm.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.c) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/aesGcm.o ../crypto-core/aesGcm.c

${OBJECTDIR}/_ext/1472/CvString.o: ../CvString.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvString.o ../CvString.cpp

${OBJECTDIR}/_ext/1472/CvStrongRng.o: ../CvStrongRng.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvStrongRng.o ../CvStrongRng.cpp

${OBJECTDIR}/_ext/2003432284/zzn2.o: ../crypto-core/zzn2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/zzn2.o ../crypto-core/zzn2.cpp

${OBJECTDIR}/_ext/2003432284/sakke_bn_c.o: ../crypto-core/sakke_bn_c.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.c) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/sakke_bn_c.o ../crypto-core/sakke_bn_c.c

${OBJECTDIR}/_ext/1472/CvEccsi.o: ../CvEccsi.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvEccsi.o ../CvEccsi.cpp

${OBJECTDIR}/_ext/2003432284/ecp_c.o: ../crypto-core/ecp_c.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.c) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/ecp_c.o ../crypto-core/ecp_c.c

${OBJECTDIR}/_ext/2003432284/zzn12a.o: ../crypto-core/zzn12a.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/zzn12a.o ../crypto-core/zzn12a.cpp

${OBJECTDIR}/_ext/2003432284/ecdh_c.o: ../crypto-core/ecdh_c.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.c) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/ecdh_c.o ../crypto-core/ecdh_c.c

${OBJECTDIR}/_ext/1472/CvMikey.o: ../CvMikey.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvMikey.o ../CvMikey.cpp

${OBJECTDIR}/_ext/2003432284/zzn.o: ../crypto-core/zzn.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/zzn.o ../crypto-core/zzn.cpp

${OBJECTDIR}/_ext/2003432284/zzn4.o: ../crypto-core/zzn4.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/zzn4.o ../crypto-core/zzn4.cpp

${OBJECTDIR}/_ext/2003432284/ecn.o: ../crypto-core/ecn.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/ecn.o ../crypto-core/ecn.cpp

${OBJECTDIR}/_ext/1472/CvSakke.o: ../CvSakke.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvSakke.o ../CvSakke.cpp

${OBJECTDIR}/_ext/2003432284/pfc.o: ../crypto-core/pfc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/pfc.o ../crypto-core/pfc.cpp

${OBJECTDIR}/_ext/2003432284/csprng_c.o: ../crypto-core/csprng_c.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.c) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/csprng_c.o ../crypto-core/csprng_c.c

${OBJECTDIR}/_ext/1472/CvEcdh.o: ../CvEcdh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvEcdh.o ../CvEcdh.cpp

${OBJECTDIR}/_ext/2003432284/ecn2.o: ../crypto-core/ecn2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2003432284
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../../MIRACL -I../include -I../crypto-core -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2003432284/ecn2.o ../crypto-core/ecn2.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/crypto-core-test

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
