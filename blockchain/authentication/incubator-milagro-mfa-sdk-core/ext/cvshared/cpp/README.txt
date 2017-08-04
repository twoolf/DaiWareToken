The C++ shared components code might now be used by any of you for your C++ projects.

The idea is to:
 - Reuse tested and robust code for faster development
 - Unify the C++ code in the company, so when any of us switches projects, he will feel “at home”
   with the other developer’s code.
 - Make the usage of some cumbersome C (or C++) libraries or OS system calls, easier for the developer.

Another idea behind the shared components code is to extend it in the future with any generic code that might
be useful for other developers.
If as part of your development you write such generic code that might be re-used, please feel free to add it.
In opposite, please keep away code that has specifics in it and is not generic enough.

If you discover any bugs, feel free to fix them in-place.

NOTE: Currently the code in the shared components is written for, and tested on Linux only.
Further development will be required to make them compatible for Windows as well.

The shared code is provided as C++ classes. You should include the relevant files into your projects,
compile them and link with the relevant 3rd party libraries, which those classes wrap.

NOTE: Examples for usage of most of the classes might be found in the project CVSHARED-CPP-TEST, in its main.cpp

What could be found in that repository:

CvCommon.h – just some common definitions used through-out all the shared components and are recommended
for usage in application layer as well.

CvLogger.h, CvLogger.cpp – Logging facility, built on top of Linux syslog facility. Used by most of the other
shared components.

CvTime.h, CvTime.cpp – Convenient time and date classes, which makes it very easy to convert between different
time units and time struct’s used by Linux. You should link with librt.so.

CvThread.h, CvThread.cpp – Wraps the Linux pthread facility and makes it easier to handle Linux threads.

CvMutex.h, CvMutex.cpp – Wraps the Linux pthread_mutex facility and makes it easier to define and use mutexes.
Additionally, defines a scope sensitive CvMutexLock class, which locks a mutex in its constructor and releases
it in the destructor.

CvString.h, CvString.cpp – Adds functionality to the STL std::string. There are no additional members, just
additional methods. Between the added functionality – Character left/right trimming, character and sub-string
replacing, parsing into tokens, formatting in similar to the printf() function, converting from/to integers.

CvXcode.h, CvXcode.cpp – Base64 and Hex encoding/decoding. Base64 code was borrowed from the MIKEYSAKKE library.

json/ - 3rd party CAJUN JSON library, which might be used “as-is”. It is already used in several projects in the
company.

CvXml.h, xml/ - 3rd party RapidXML XML parser/composer library, “wrapped” by some definitions in the CvXml.h file.
Include the last for more convenient usage.

CvHttpCommon.h – common definitions for the CvHttpServer and CvHttpRequest classes.

CvHttpRequest.h, CvHttpRequest.cpp – wrapper for the CURL library. If you use it you should link with libcurl.so

CvHttpServer.h, CvHttpServer.cpp - Abstract HTTP server class, which serves as a base for the CvHttpServerUv and
CvHttpServerMg. It also defines some subclasses that are used by both server implementations. If you use any of the
inheriting classes, you should include in your project also the CvHttpServer.cpp file.

CvHttpServerMg.h, CvHttpServerMg.cpp – wrapper for the Mongoose HTTP server, which is thread-based. If you use this
class (you should actually inherit it), you should also include into your project the mongoose/mongoose.c file.
It appears that mongoose.c requires link with libdl.so

CvHttpServerUv.h, CvHttpServerUv.cpp – HTTP server based on the libuv, which is non-blocking and single-thread.
If you use this class (you should actually inherit it), your requests handler should be non-blocking, otherwise it
will block the operation of the whole server. When you use this class, make sure that you compile the libuv under
the folder ./libuv and the http-parser under the folder ./http-parser. Both should be compiled using their makefiles,
executing the "make" command. Then you should link your project with ./libuv/uv.a and ./http-parser/http_parser.o

CvLdapConnection.h, CvLdapConnection.cpp – Wraps the standard libldap.so and provides easy connection to an LDAP
server and search in an LDAP directory. You should link with libldap.so, of course.

CvCouchDb.h, CvCouchDb.cpp – Easy access to a CouchDB, still through HTTP. Uses the CvHttpRequest class to access
the CouchDB and the JSON library to handle documents and responses.

CvRabbitMq.h, CvRabbitMq.cpp – Wraps the librabbitmq.so for easy read/write from/to a RabbitMQ. You should link with
librabbitmq.so, of course.

CvStateMachine.h, CvStateMachine.cpp - Generic state machine implementation

CvSemaphore.h, CvSemaphore.cpp – Wraps the Linux semaphore facility and makes it easier to define and use semaphores.
Additionally, defines a scope sensitive CvSemaphoreLock class, which locks a semaphore in its constructor and releases
it in the destructor.

CvQueue.h - A template for a message queue, which might be very easily used to exchange information  between the
threads of a process.

CvCondVar.h, CvCondVar.cpp - Wrapper for the Linux pthread conditional variable facility, which makes it much easier to
define and work with conditional variables. It encapsulates the whole mechanism, including the mutex that works in
conjunction with the conditional variable.

Have fun,
Mony