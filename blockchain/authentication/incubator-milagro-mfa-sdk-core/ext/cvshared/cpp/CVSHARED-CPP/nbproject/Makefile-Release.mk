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
	${OBJECTDIR}/_ext/2111058971/CvLogger.o \
	${OBJECTDIR}/_ext/1472/CvHttpServer.o \
	${OBJECTDIR}/_ext/1472/CvHttpServerUv.o \
	${OBJECTDIR}/_ext/2111058971/CvFileSystem.o \
	${OBJECTDIR}/_ext/2111058971/CvCondVar.o \
	${OBJECTDIR}/_ext/1472/CvHttpRequestAsync.o \
	${OBJECTDIR}/_ext/2111058971/CvTimer.o \
	${OBJECTDIR}/_ext/1472/CvString.o \
	${OBJECTDIR}/_ext/2111058971/CvThread.o \
	${OBJECTDIR}/_ext/1472/CvStateMachine.o \
	${OBJECTDIR}/_ext/2111058971/CvHttpRequest.o \
	${OBJECTDIR}/_ext/1472/CvCouchDb.o \
	${OBJECTDIR}/_ext/2111058971/CvRabbitMq.o \
	${OBJECTDIR}/_ext/2111058971/CvSemaphore.o \
	${OBJECTDIR}/_ext/1472/CvTime.o \
	${OBJECTDIR}/_ext/2111058971/CvMutex.o \
	${OBJECTDIR}/_ext/886878980/mongoose.o \
	${OBJECTDIR}/_ext/2111058971/CvLdapConnection.o \
	${OBJECTDIR}/_ext/1472/CvHttpServerMg.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcvshared-cpp.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcvshared-cpp.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcvshared-cpp.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcvshared-cpp.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcvshared-cpp.a

${OBJECTDIR}/_ext/1472/CvXcode.o: ../CvXcode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvXcode.o ../CvXcode.cpp

${OBJECTDIR}/_ext/2111058971/CvLogger.o: ../linux/CvLogger.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvLogger.o ../linux/CvLogger.cpp

${OBJECTDIR}/_ext/1472/CvHttpServer.o: ../CvHttpServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvHttpServer.o ../CvHttpServer.cpp

${OBJECTDIR}/_ext/1472/CvHttpServerUv.o: ../CvHttpServerUv.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvHttpServerUv.o ../CvHttpServerUv.cpp

${OBJECTDIR}/_ext/2111058971/CvFileSystem.o: ../linux/CvFileSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvFileSystem.o ../linux/CvFileSystem.cpp

${OBJECTDIR}/_ext/2111058971/CvCondVar.o: ../linux/CvCondVar.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvCondVar.o ../linux/CvCondVar.cpp

${OBJECTDIR}/_ext/1472/CvHttpRequestAsync.o: ../CvHttpRequestAsync.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvHttpRequestAsync.o ../CvHttpRequestAsync.cpp

${OBJECTDIR}/_ext/2111058971/CvTimer.o: ../linux/CvTimer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvTimer.o ../linux/CvTimer.cpp

${OBJECTDIR}/_ext/1472/CvString.o: ../CvString.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvString.o ../CvString.cpp

${OBJECTDIR}/_ext/2111058971/CvThread.o: ../linux/CvThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvThread.o ../linux/CvThread.cpp

${OBJECTDIR}/_ext/1472/CvStateMachine.o: ../CvStateMachine.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvStateMachine.o ../CvStateMachine.cpp

${OBJECTDIR}/_ext/2111058971/CvHttpRequest.o: ../linux/CvHttpRequest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvHttpRequest.o ../linux/CvHttpRequest.cpp

${OBJECTDIR}/_ext/1472/CvCouchDb.o: ../CvCouchDb.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvCouchDb.o ../CvCouchDb.cpp

${OBJECTDIR}/_ext/2111058971/CvRabbitMq.o: ../linux/CvRabbitMq.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvRabbitMq.o ../linux/CvRabbitMq.cpp

${OBJECTDIR}/_ext/2111058971/CvSemaphore.o: ../linux/CvSemaphore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvSemaphore.o ../linux/CvSemaphore.cpp

${OBJECTDIR}/_ext/1472/CvTime.o: ../CvTime.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvTime.o ../CvTime.cpp

${OBJECTDIR}/_ext/2111058971/CvMutex.o: ../linux/CvMutex.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvMutex.o ../linux/CvMutex.cpp

${OBJECTDIR}/_ext/886878980/mongoose.o: ../mongoose/mongoose.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/886878980
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/886878980/mongoose.o ../mongoose/mongoose.c

${OBJECTDIR}/_ext/2111058971/CvLdapConnection.o: ../linux/CvLdapConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2111058971
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/2111058971/CvLdapConnection.o ../linux/CvLdapConnection.cpp

${OBJECTDIR}/_ext/1472/CvHttpServerMg.o: ../CvHttpServerMg.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../include -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/CvHttpServerMg.o ../CvHttpServerMg.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcvshared-cpp.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
