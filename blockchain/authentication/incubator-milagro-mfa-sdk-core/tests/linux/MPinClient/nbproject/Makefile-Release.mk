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
	${OBJECTDIR}/_ext/620013993/CvString.o \
	${OBJECTDIR}/_ext/620013993/CvXcode.o \
	${OBJECTDIR}/_ext/580510450/CvHttpRequest.o \
	${OBJECTDIR}/_ext/580510450/CvLogger.o \
	${OBJECTDIR}/_ext/580510450/CvMutex.o \
	${OBJECTDIR}/_ext/580510450/CvSemaphore.o \
	${OBJECTDIR}/_ext/580510450/CvThread.o \
	${OBJECTDIR}/_ext/605162843/aes.o \
	${OBJECTDIR}/_ext/605162843/big.o \
	${OBJECTDIR}/_ext/605162843/ecp.o \
	${OBJECTDIR}/_ext/605162843/ecp2.o \
	${OBJECTDIR}/_ext/605162843/ff.o \
	${OBJECTDIR}/_ext/605162843/fp.o \
	${OBJECTDIR}/_ext/605162843/fp12.o \
	${OBJECTDIR}/_ext/605162843/fp2.o \
	${OBJECTDIR}/_ext/605162843/fp4.o \
	${OBJECTDIR}/_ext/605162843/gcm.o \
	${OBJECTDIR}/_ext/605162843/hash.o \
	${OBJECTDIR}/_ext/605162843/mpin.o \
	${OBJECTDIR}/_ext/605162843/oct.o \
	${OBJECTDIR}/_ext/605162843/pair.o \
	${OBJECTDIR}/_ext/605162843/rand.o \
	${OBJECTDIR}/_ext/605162843/rom.o \
	${OBJECTDIR}/_ext/605162843/version.o \
	${OBJECTDIR}/_ext/1386528437/mpin_crypto_non_tee.o \
	${OBJECTDIR}/_ext/1386528437/mpin_sdk.o \
	${OBJECTDIR}/_ext/1386528437/utils.o \
	${OBJECTDIR}/HttpRequest.o \
	${OBJECTDIR}/MpinClient.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread -lcurl -lcrypto

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mpinclient

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mpinclient: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mpinclient ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/620013993/CvString.o: ../../../ext/cvshared/cpp/CvString.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/620013993
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/620013993/CvString.o ../../../ext/cvshared/cpp/CvString.cpp

${OBJECTDIR}/_ext/620013993/CvXcode.o: ../../../ext/cvshared/cpp/CvXcode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/620013993
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/620013993/CvXcode.o ../../../ext/cvshared/cpp/CvXcode.cpp

${OBJECTDIR}/_ext/580510450/CvHttpRequest.o: ../../../ext/cvshared/cpp/linux/CvHttpRequest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/580510450
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/580510450/CvHttpRequest.o ../../../ext/cvshared/cpp/linux/CvHttpRequest.cpp

${OBJECTDIR}/_ext/580510450/CvLogger.o: ../../../ext/cvshared/cpp/linux/CvLogger.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/580510450
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/580510450/CvLogger.o ../../../ext/cvshared/cpp/linux/CvLogger.cpp

${OBJECTDIR}/_ext/580510450/CvMutex.o: ../../../ext/cvshared/cpp/linux/CvMutex.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/580510450
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/580510450/CvMutex.o ../../../ext/cvshared/cpp/linux/CvMutex.cpp

${OBJECTDIR}/_ext/580510450/CvSemaphore.o: ../../../ext/cvshared/cpp/linux/CvSemaphore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/580510450
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/580510450/CvSemaphore.o ../../../ext/cvshared/cpp/linux/CvSemaphore.cpp

${OBJECTDIR}/_ext/580510450/CvThread.o: ../../../ext/cvshared/cpp/linux/CvThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/580510450
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/580510450/CvThread.o ../../../ext/cvshared/cpp/linux/CvThread.cpp

${OBJECTDIR}/_ext/605162843/aes.o: ../../../src/crypto/aes.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/aes.o ../../../src/crypto/aes.c

${OBJECTDIR}/_ext/605162843/big.o: ../../../src/crypto/big.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/big.o ../../../src/crypto/big.c

${OBJECTDIR}/_ext/605162843/ecp.o: ../../../src/crypto/ecp.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/ecp.o ../../../src/crypto/ecp.c

${OBJECTDIR}/_ext/605162843/ecp2.o: ../../../src/crypto/ecp2.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/ecp2.o ../../../src/crypto/ecp2.c

${OBJECTDIR}/_ext/605162843/ff.o: ../../../src/crypto/ff.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/ff.o ../../../src/crypto/ff.c

${OBJECTDIR}/_ext/605162843/fp.o: ../../../src/crypto/fp.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/fp.o ../../../src/crypto/fp.c

${OBJECTDIR}/_ext/605162843/fp12.o: ../../../src/crypto/fp12.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/fp12.o ../../../src/crypto/fp12.c

${OBJECTDIR}/_ext/605162843/fp2.o: ../../../src/crypto/fp2.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/fp2.o ../../../src/crypto/fp2.c

${OBJECTDIR}/_ext/605162843/fp4.o: ../../../src/crypto/fp4.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/fp4.o ../../../src/crypto/fp4.c

${OBJECTDIR}/_ext/605162843/gcm.o: ../../../src/crypto/gcm.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/gcm.o ../../../src/crypto/gcm.c

${OBJECTDIR}/_ext/605162843/hash.o: ../../../src/crypto/hash.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/hash.o ../../../src/crypto/hash.c

${OBJECTDIR}/_ext/605162843/mpin.o: ../../../src/crypto/mpin.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/mpin.o ../../../src/crypto/mpin.c

${OBJECTDIR}/_ext/605162843/oct.o: ../../../src/crypto/oct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/oct.o ../../../src/crypto/oct.c

${OBJECTDIR}/_ext/605162843/pair.o: ../../../src/crypto/pair.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/pair.o ../../../src/crypto/pair.c

${OBJECTDIR}/_ext/605162843/rand.o: ../../../src/crypto/rand.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/rand.o ../../../src/crypto/rand.c

${OBJECTDIR}/_ext/605162843/rom.o: ../../../src/crypto/rom.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/rom.o ../../../src/crypto/rom.c

${OBJECTDIR}/_ext/605162843/version.o: ../../../src/crypto/version.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/605162843
	${RM} "$@.d"
	$(COMPILE.c) -O2 -I../../../src/crypto -I../../../src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/605162843/version.o ../../../src/crypto/version.c

${OBJECTDIR}/_ext/1386528437/mpin_crypto_non_tee.o: ../../../src/mpin_crypto_non_tee.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1386528437/mpin_crypto_non_tee.o ../../../src/mpin_crypto_non_tee.cpp

${OBJECTDIR}/_ext/1386528437/mpin_sdk.o: ../../../src/mpin_sdk.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1386528437/mpin_sdk.o ../../../src/mpin_sdk.cpp

${OBJECTDIR}/_ext/1386528437/utils.o: ../../../src/utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1386528437
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1386528437/utils.o ../../../src/utils.cpp

${OBJECTDIR}/HttpRequest.o: HttpRequest.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HttpRequest.o HttpRequest.cpp

${OBJECTDIR}/MpinClient.o: MpinClient.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MpinClient.o MpinClient.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../../src/crypto -I../../../src -I../../../ext/cvshared/cpp/include -I../../../src/json -I../../../src/utf8 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mpinclient

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
